/* Aftershock 3D rendering engine
 * Copyright (C) 1999 Stephen C. Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#define WIN32_LEAN_AND_MEAN

#include "a_shared.h"
#include "cmap.h"
#include "shader.h"
#include "render.h"
#include "mesh.h"
#include "skybox.h"
#include "md3.h"
#include "mapent.h"
#include "renderback.h"
#include "opengl.h"
#include "console.h"
#include <math.h>
#include "matrix.h"

/* The back-end of the rendering pipeline does the actual drawing.
 * All triangles which share a rendering state (shader) are pushed together
 * into the 'arrays' structure.  This includes verts, texcoords, and element
 * numbers.  The renderer is then 'flushed': the rendering state is set
 * and the triangles are drawn.  The arrays and rendering state is then
 * cleared for the next set of triangles.
 */

/* FIXME: The manner in which faces are "pushed" to the arrays is really
   absimal.  I'm sure it could be highly optimized. */

/* FIXME: It would be nice to have a consistent view of faces, meshes,
   mapents, etc. so we don't have to have a "push" function for each. */


void R_BackendFlush (int shadernum, int lmtex);
static void R_BackendFlush_Generic (shader_t *s, int lmtex);
static void R_BackendFlush_MultitextureLightmapped (shader_t *s, int lmtex);
static void R_BackendFlush_MultitextureCombine (shader_t *s, int lmtex);
static void R_BackendFlush_VertexLit (shader_t *s, int lmtex);

static void R_BackendPushFace(cface_t *face);
static void R_BackendPushMesh(mesh_t *mesh);
void		R_BackendPushPoly(poly_t *p);
static void R_BackendStripmine(int numelems, int *elems);

typedef struct
{
    int			numverts;
    vec3_t		*verts;
	vec3_t		*norms;
    colour_t	*colour;
	colour_t	*entity_colour;
    vec2_t		*tex_st;
    vec2_t		*texmod_st;
    vec2_t		*lm_st;
	vec2_t		**stage_tex_st;
	colour_t	*mod_colour;
    int			numelems;
    int			*elems;
} arrays_t;

static arrays_t arrays;

aboolean r_overlay = afalse;
colour_t r_actcolor = {255, 255, 255, 255};

extern reference_t transform_ref;

static float turbsin[256];
static float sintable[1024];
static float modmat[16];

static float render_func_eval(uint_t func, float *args)
{
    float x = args[2] + shadertime * args[3];
	x -= floor(x);

	// Evaluate a number of time based periodic functions
	switch (func)
	{
	case SHADER_FUNC_SIN:
		return sintable[(int)(x * 1024.0f) & 1023] * args[1] + args[0];
		
	case SHADER_FUNC_TRIANGLE:
		return (x < 0.5) ? (2.0 * x - 1.0) * args[1] + args[0] : 
		(-2.0 * x + 2.0) * args[1] + args[0];
		
	case SHADER_FUNC_SQUARE:
		return (x < 0.5) ? args[1] + args[0] : args[0] - args[1];
		
	case SHADER_FUNC_SAWTOOTH:
		return x * args[1] + args[0];
		
	case SHADER_FUNC_INVERSESAWTOOTH:
		return (1.0 - x) * args[1] + args[0];
	}

    return 0.0;
}

void R_BackendInit(void)
{
	int i;

    arrays.verts = (vec3_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
	arrays.norms = (vec3_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
    arrays.tex_st = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
    arrays.texmod_st = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
	arrays.lm_st = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
	arrays.elems = (int *)malloc(MAX_ARRAYS_ELEMS * sizeof(int));
    arrays.colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.mod_colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.entity_colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));	

	if (!r_allowExtensions->integer)
	{
		arrays.stage_tex_st = (vec2_t **)malloc(sizeof(vec2_t *));
		arrays.stage_tex_st[0] = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
		glconfig.maxActiveTextures = 1;
	}
	else 
	{
		arrays.stage_tex_st = (vec2_t **)malloc(glconfig.maxActiveTextures * sizeof(vec2_t *));
		
		for (i = 0; i < glconfig.maxActiveTextures; i++)
			arrays.stage_tex_st[i] = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
	}

	for (i = 0; i < 1024; i++)
	{
		if (i < 256)
			turbsin[i] = (float)sin(i * (TWOPI / 256.0f));

		sintable[i] = (float)sin(i * (TWOPI / 1024.0f));
	}
}

void R_BackendShutdown(void)
{
	int i;

	free(arrays.verts);
    free(arrays.tex_st);
    free(arrays.texmod_st);
    free(arrays.lm_st);
	free(arrays.norms);
    free(arrays.elems);
    free(arrays.colour);
	free(arrays.mod_colour);

	for (i = 0; i < glconfig.maxActiveTextures; i++)
		free (arrays.stage_tex_st[i]);

	free (arrays.stage_tex_st);
}

void R_BackendPushQuad (quad_t *q)
{
	R_BackendClear();

	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[0];
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[1];
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[2];
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[3];
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[4];
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[5];

	VectorCopy(q->verts[0], arrays.verts[arrays.numverts]);
	Vector2Copy(q->tc[0], arrays.tex_st[arrays.numverts]);
	Vector4Copy(q->color[0], arrays.colour[arrays.numverts]);
	arrays.numverts++;

	VectorCopy(q->verts[1], arrays.verts[arrays.numverts]);
	Vector2Copy(q->tc[1], arrays.tex_st[arrays.numverts]);
	Vector4Copy(q->color[1], arrays.colour[arrays.numverts]);
	arrays.numverts++;

	VectorCopy(q->verts[2], arrays.verts[arrays.numverts]);
	Vector2Copy(q->tc[2], arrays.tex_st[arrays.numverts]);
	Vector4Copy(q->color[2], arrays.colour[arrays.numverts]);
	arrays.numverts++;

	VectorCopy(q->verts[3], arrays.verts[arrays.numverts]);
	Vector2Copy(q->tc[3], arrays.tex_st[arrays.numverts]);
	Vector4Copy(q->color[3], arrays.colour[arrays.numverts]);
	arrays.numverts++;
}

void R_BackendPushPoly (poly_t *p)
{
	int j;

	for (j = 0; j < p->numVerts; j++, arrays.numverts++)
	{
		VectorCopy(p->verts[j].xyz, arrays.verts[arrays.numverts]);
		Vector2Copy(p->verts[j].st, arrays.tex_st[arrays.numverts]);
		Vector4Copy(p->verts[j].modulate, arrays.colour[j]);
		arrays.elems[arrays.numverts] = arrays.numverts + j;
	}
}

void R_BackendOverlay (quad_t *q, int numquads)
{
	int i, shader = q->shader;
	quad_t *quad = &q[0];

	for (i = 0; i < numquads; i++, q++)
    {
		// Look for quads that share rendering state
		if (shader != quad->shader)
		{
			// Flush the renderer and reset
			R_BackendFlush(shader, 0);
			shader = quad->shader;
		}

		R_BackendPushQuad (quad);
    }

    // Final flush to clear queue
	R_BackendFlush(shader, 0);
}

void Render_Backend(facelist_t *facelist)
{
    int f, shader = 0, lmtex = 0;
    uint_t key = (uint_t)-1;
    cface_t *face;   

   	R_BackendClear();

    for (f = 0; f < facelist->numfaces; f++)
    {
		face = &cm.faces[facelist->faces[f].face];

		// Look for faces that share rendering state
		if (facelist->faces[f].sortkey != key)
		{
			// Flush the renderer and reset
			if (f) R_BackendFlush(shader, lmtex);
			shader = face->shadernum;
			lmtex = face->lightmapnum;
			key = facelist->faces[f].sortkey;
		}

		// Push the face to the triangle arrays
		switch (face->facetype)
		{
			case FACETYPE_NORMAL:
			case FACETYPE_TRISURF:
				R_BackendPushFace(face);
				break;

			case FACETYPE_MESH:
				R_BackendPushMesh(&r_meshes[r_facemeshes[facelist->faces[f].face]]);
				break;

			default:
				break;
		}
    }

    // Final flush to clear queue
	R_BackendFlush(shader, lmtex);
}

void R_SetColor (const float *rgba)
{
	if (!rgba)
	{
		ClearColor (r_actcolor);
		return;
	}
 
	r_actcolor[0] = FloatToByte(rgba[0]*255.0f);
	r_actcolor[1] = FloatToByte(rgba[1]*255.0f);
	r_actcolor[2] = FloatToByte(rgba[2]*255.0f);
	r_actcolor[3] = FloatToByte(rgba[3]*255.0f);
}

void R_BackendClearColor( void )
{
	memset (arrays.colour, 255, sizeof(colour_t)*arrays.numverts);
}

void R_BackendClearEntityColor( void )
{
	memset (arrays.entity_colour, 255, sizeof(colour_t)*arrays.numverts);
}

void R_BackendSetEntityColor( const colour_t c )
{
	int i;

	for( i = 0; i < arrays.numverts; i++ )
		Vector4Copy (c, arrays.entity_colour[i]);
}

void R_BackendSky(int numsky, int *skylist)
{
    int s, i, shader = cm.faces[skylist[0]].shadernum;
    float skyheight = r_shaders[shader].skyheight;
    uint_t *elem;

	R_BackendClear();

    // Center skybox on camera to give the illusion of a larger space
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(r_eyepos[0], r_eyepos[1], r_eyepos[2]);
    glScalef(skyheight, skyheight, skyheight);

    // FIXME: Need to cull skybox based on face list
    for (s = 0; s < 5; s++)
    {
		elem = r_skybox->elems;

		for (i = 0; i < r_skybox->numelems; i++)
			arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;

		for (i = 0; i < r_skybox->numpoints; i++, arrays.numverts++)
		{
			VectorCopy(r_skybox->points[s][i], arrays.verts[arrays.numverts]);
			Vector2Copy(r_skybox->tex_st[s][i], arrays.tex_st[arrays.numverts]);
		}
    }

	R_BackendFlush(shader, 0);
	
    // Restore world space
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


static void R_BackendPushFace(cface_t *face)
{
    int *elem = face->elems;
    cvertex_t *vert = face->verts;
	int i;

	if (arrays.numverts >= MAX_ARRAYS_VERTS)
		return;

    for (i = 0; i < face->numelems; i++)
		arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
    
    for (i = 0; i < face->numverts; i++, vert++, arrays.numverts++)
	{
		VectorCopy(vert->v_point, arrays.verts[arrays.numverts]);
		VectorCopy(vert->v_norm, arrays.norms[arrays.numverts]);
		Vector2Copy(vert->tex_st, arrays.tex_st[arrays.numverts]);
		Vector2Copy(vert->lm_st, arrays.lm_st[arrays.numverts]);
		Vector4Copy(vert->colour, arrays.colour[arrays.numverts]);
    }	    
}

void R_BackendInterpolateNormals( vec3_t *n1, vec3_t *n2, float frac, int numverts )
{
	int i = arrays.numverts - numverts;

	for( ; i < arrays.numverts; i++ ) {
		R_InterpolateNormal (n1[i], n2[i], frac, arrays.verts[i]);
	}
}

void R_BackendPushRaw( vec3_t *v, vec3_t *n, vec2_t *tc, int *elems, int numverts, int numelems )
{
	int i;

	if( arrays.numverts >= MAX_ARRAYS_VERTS )
		return;

	for( i = 0; i < numelems; i++ )
		arrays.elems[arrays.numelems++] = arrays.numverts + *elems++;

	for( i = 0; i < numverts; i++, arrays.numverts++ ) {
		VectorCopy( v[i], arrays.verts[arrays.numverts] );
		VectorCopy( n[i], arrays.norms[arrays.numverts] );
		Vector2Copy( tc[i], arrays.tex_st[arrays.numverts] );
	}
}


static void R_BackendPushMesh(mesh_t *mesh)
{
    int  i, *elem = mesh->elems;

	if (arrays.numverts >= MAX_ARRAYS_VERTS)
		return;

	// any way to implement faceculling here in the engine ?
    for (i = 0; i < mesh->numelems; i++)
		arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	
	for (i = 0; i < mesh->msize; i++, arrays.numverts++)
	{
	    VectorCopy(mesh->points[i], arrays.verts[arrays.numverts]);
	    Vector2Copy(mesh->tex_st[i], arrays.tex_st[arrays.numverts]);
	    Vector2Copy(mesh->lm_st[i], arrays.lm_st[arrays.numverts]);
	}
}

static void R_BackendStripmine(int numelems, int *elems)
{
    int toggle;
    uint_t a, b, elem;

    /* Vertexes are in tristrip order where possible.  If we can't lock
     * the vertex arrays (glLockArraysEXT), then it's better to send
     * tristrips instead of triangles (less transformations).
     * This function looks for and sends tristrips.
     */

    /* Tristrip order elems look like this:
     *  0 1 2 2 1 3 2 3 4 4 3 5 4 5 7 7 5 6  <-- elems
     *    b a a b b a b a a b b a b a a b b  <-- ab pattern
     *    \ 1 / \ 2 / \ 3 / \ 4 / \ 5 /      <-- baa/bba groups
     */
    
    elem = 0;
    while (elem < numelems)
    {
		toggle = 1;
		glBegin(GL_TRIANGLE_STRIP);
		
		glArrayElement(elems[elem++]);
		b = elems[elem++];
		glArrayElement(b);
		a = elems[elem++];
		glArrayElement(a);
		
		while (elem < numelems)
		{
			if (a != elems[elem] || b != elems[elem+1])
				break;
			
			if (toggle)
			{
				b = elems[elem+2];
				glArrayElement(b);
			}
			else
			{
				a = elems[elem+2];
				glArrayElement(a);
			}

			elem += 3;
			toggle = !toggle;
		}

		glEnd();
    }
}

// TODO !!!
void R_BackendMake_Vertices (shader_t *s)
{
	float deflect;
	vec3_t v;
	int i, n;
	float args[4], startoff, off, wavesize;

	if ((s->flags & SHADER_DEFORMVERTS) && s->numdeforms)
	{
		for (n = 0; n < s->numdeforms; n++)
		{
			switch (s->deform_vertices[n])
			{
				case DEFORMV_NONE:
					break;

				case DEFORMV_WAVE:
					args[0] = s->deformv_wavefunc[n].args[0];
					args[1] = s->deformv_wavefunc[n].args[1];
					args[2] = 0;
					args[3] = s->deformv_wavefunc[n].args[3];
					startoff = s->deformv_wavefunc[n].args[2];
					wavesize = s->deform_params[n][0];
					
					for (i = 0; i < arrays.numverts; i++)
					{
						// FIXME: this clearly isn't the way deform waves are applied to
						// world coordinates.  For now, it at least waves the banners :)
						VectorCopy (arrays.verts[i], v);
						off = VectorNormalize(v) * wavesize;
						
						// Evaluate wave function
						args[2] = startoff + off;
						deflect = render_func_eval(s->deformv_wavefunc[n].func, args);
						
						// Deflect vertex along its normal by wave amount
						VectorScale(arrays.norms[i], deflect, v);
						VectorAdd(v, arrays.verts[i], arrays.verts[i]);
					}
					break;

				case DEFORMV_NORMAL:
					deflect = s->deform_params[n][0];

					for (i = 0; i < arrays.numverts; i++)
					{
						VectorCopy(arrays.norms[i], v);
						VectorScale(v, deflect, v);
						VectorCopy(v, arrays.norms[i]);
						VectorAdd(v, arrays.verts[i], arrays.verts[i]);
					}
					break;

				case DEFORMV_MOVE:
					deflect = render_func_eval(s->movev_wavefunc[n].func, s->movev_wavefunc[n].args);
					VectorCopy (s->deform_params[n], v); 
					VectorScale (v, deflect, v);
					VectorAdd(v, arrays.verts[i], arrays.verts[i]);
					break;

				case DEFORMV_BULGE:
					break;

				case DEFORMV_AUTOSPRITE:
					break;

				case DEFORMV_AUTOSPRITE2:
					break;

				default:
					break;
			}
		}
	}
}

float *R_BackendMake_TexCoords (shaderpass_t *pass, int stage)
{
	int i = arrays.numverts;
	vec2_t *in = arrays.tex_st;
	vec2_t *out = arrays.stage_tex_st[stage];
	vec2_t *txmod = arrays.texmod_st;

	switch (pass->tc_gen)
	{
		case TC_GEN_BASE:
			in = arrays.tex_st;
			break;

		case TC_GEN_LIGHTMAP:
			in = arrays.lm_st;
			break;
	
		case TC_GEN_ENVIRONMENT:
		{
			// TODO !!!
			vec3_t dir;
			int j;

			in = arrays.stage_tex_st[stage];

			for (j = 0; j < arrays.numverts; j++)
			{
				dir[0] = (arrays.verts[j][0] * modmat[0]) + (arrays.verts[j][1] * modmat[4]) + (arrays.verts[j][2] * modmat[8] ) + modmat[12];
				dir[1] = (arrays.verts[j][0] * modmat[1]) + (arrays.verts[j][1] * modmat[5]) + (arrays.verts[j][2] * modmat[9] ) + modmat[13];
				dir[2] = (arrays.verts[j][0] * modmat[2]) + (arrays.verts[j][1] * modmat[6]) + (arrays.verts[j][2] * modmat[10]) + modmat[14];

				VectorNormalize (dir);

				in[j][0] = dir[2];
				in[j][1] = dir[1];
			}
		}
		break;

		case TC_GEN_VECTOR:
		{
			int j;
			in = arrays.stage_tex_st[stage];

			for (j = 0;j < arrays.numverts; j++)
			{ 
				in[j][0] = DotProduct(pass->tc_gen_s, arrays.verts[j]);
				in[j][1] = DotProduct(pass->tc_gen_t, arrays.verts[j]);
			}
		}

		break;

		default:
			in = arrays.tex_st;
			break;
	}

	if ((pass->tc_gen != TC_GEN_ENVIRONMENT) &&
		(pass->tc_gen != TC_GEN_VECTOR))
		memcpy (out, in, arrays.numverts * sizeof(vec2_t));

    if (pass->flags & SHADER_TCMOD)
	{
		float t1, t2, p1, p2;
		float sint, cost;
		int n, j;

		for (n = 0; n < pass->num_tc_mod; n++)
		{
			memcpy (txmod, out, arrays.numverts * sizeof(vec2_t));

			switch (pass->tc_mod[n].type)
			{
				case SHADER_TCMOD_ROTATE:
				{
					j = (int)(-pass->tc_mod[n].args[0] * shadertime * 1024 / 360.0f);
					
					sint = sintable[j & 1023];
					cost = sintable[(j + 256) & 1023];
					p1 = 0.5f - cost * 0.5f + sint * 0.5f;
					p2 = 0.5f - sint * 0.5f - cost * 0.5f;
					
					for (j = 0; j < arrays.numverts; j++)
					{
						t1 = txmod[j][0];
						t2 = txmod[j][1];
						
						out[j][0] = t1 * cost - t2 * sint + p1;
						out[j][1] = t2 * cost + t1 * sint + p2;
					}
				}
				break;
				
				case SHADER_TCMOD_SCALE:
				{
					t1 = pass->tc_mod[n].args[0];
					t2 = pass->tc_mod[n].args[1];

					for (j = 0; j < arrays.numverts; j++)
					{
						out[j][0] = txmod[j][0] * t1;
						out[j][1] = txmod[j][1] * t2;
					}
				}
				break;

				case SHADER_TCMOD_TURB:
				{
					t1 = pass->tc_mod[n].args[2] + shadertime * pass->tc_mod[n].args[3];
					t1 -= floor(t1);
					t2 = pass->tc_mod[n].args[1];

					for (j = 0; j < arrays.numverts; j++)
					{
						out[j][0] = txmod[j][0] + t2*turbsin[(int)((t1+txmod[j][1]*t2)*256.0f)&255];
						out[j][1] = txmod[j][1] + t2*turbsin[(int)((t1+txmod[j][0]*t2)*256.0f)&255];
					}
				}
				break;
			
				case SHADER_TCMOD_STRETCH:
				{
					t1 = 1.0f / (float)render_func_eval(pass->tc_mod_stretch.func,
						pass->tc_mod_stretch.args);
					t2 = 0.5f - t1 * 0.5f;

					for (j = 0; j < arrays.numverts; j++) {
						out[j][0] = txmod[j][0] * t1 + t2;
						out[j][1] = txmod[j][1] * t1 + t2;
					}
				}
				break;
					
				case SHADER_TCMOD_SCROLL:
				{
					t1 = pass->tc_mod[n].args[0] * shadertime;
					t1 -= floor(t1);

					t2 = pass->tc_mod[n].args[1] * shadertime;
					t2 -= floor(t2);

					for (j = 0; j < arrays.numverts; j++) {
						out[j][0] = txmod[j][0] + t1;
						out[j][1] = txmod[j][1] + t2;
					}
				}
				break;
				
				case SHADER_TCMOD_TRANSFORM:
				{
					for (j = 0; j < arrays.numverts; j++) 
					{
						t1 = txmod[j][0];
						t2 = txmod[j][1];
						out[j][0] = t1 * pass->tc_mod[n].args[0] + t2 * pass->tc_mod[n].args[2] + pass->tc_mod[n].args[4];
						out[j][1] = t2 * pass->tc_mod[n].args[1] + t1 * pass->tc_mod[n].args[3] + pass->tc_mod[n].args[5];
					}
				}
				break;
	
				default:
					memcpy (out, in, arrays.numverts * sizeof(vec2_t));
					break;
			}
		}
	}

	return *(float **)&out;
}

byte *R_BackendMake_Colors (shaderpass_t *pass)
{
	int i;
	byte c;
	colour_t *col = NULL;
	float color;

	switch (pass->rgbgen)
	{
		case RGB_GEN_IDENTITY_LIGHTING:
			c = (r_overBrightBits->integer) ? 255 : 127;

			for (i = 0; i < arrays.numverts; i++) 
			{
				arrays.mod_colour[i][0] = c;
				arrays.mod_colour[i][1] = c;
				arrays.mod_colour[i][2] = c;
			}

			col = arrays.mod_colour;
			break;

		case RGB_GEN_IDENTITY:
			for (i = 0; i < arrays.numverts; i++) 
			{
				arrays.mod_colour[i][0] = 255;
				arrays.mod_colour[i][1] = 255;
				arrays.mod_colour[i][2] = 255;
			}

			col = arrays.mod_colour;
			break;

		case RGB_GEN_WAVE:
			color = 255.0f * (float)render_func_eval(pass->rgbgen_func.func,
							pass->rgbgen_func.args);
			color = bound (0.0f, color, 255.0f);
			c = FloatToByte(color);

			for (i = 0; i < arrays.numverts;i++) 
			{
				arrays.mod_colour[i][0] = c;
				arrays.mod_colour[i][1] = c;
				arrays.mod_colour[i][2] = c;
			}

			col = arrays.mod_colour;
			break;

		case RGB_GEN_ENTITY:
			col = arrays.entity_colour;
			break;

		case RGB_GEN_ONE_MINUS_ENTITY:
			for (i = 0; i < arrays.numverts; i++)
			{
				arrays.mod_colour[i][0] = 255 - arrays.entity_colour[i][0];
				arrays.mod_colour[i][1] = 255 - arrays.entity_colour[i][1];
				arrays.mod_colour[i][2] = 255 - arrays.entity_colour[i][2];
				arrays.mod_colour[i][3] = 255;
			}
			break;

		case RGB_GEN_VERTEX:
			col = arrays.colour;
			break;

		case RGB_GEN_ONE_MINUS_VERTEX:
			for (i = 0; i < arrays.numverts; i++)
			{
				arrays.mod_colour[i][0] = 255 - arrays.colour[i][0];
				arrays.mod_colour[i][1] = 255 - arrays.colour[i][1];
				arrays.mod_colour[i][2] = 255 - arrays.colour[i][2];
			}
			col = arrays.mod_colour;
			break;

		case RGB_GEN_LIGHTING_DIFFUSE:	// TODO
			memset (arrays.mod_colour, 255, sizeof(colour_t)*arrays.numverts);
			col = arrays.mod_colour;
			break;

		default:
			col = arrays.colour;
			break;
	}

	// TODO !!!!
	switch (pass->alpha_gen)
	{
		case ALPHA_GEN_WAVE:
			color = 255.0f * (float)render_func_eval(pass->alphagen_func.func,
							pass->alphagen_func.args);
			color = bound (0.0f, color, 255.0f);
			c = FloatToByte(color);

			for (i = 0; i < arrays.numverts; i++) {
				col[i][3] = c;
			}

			break;

		case ALPHA_GEN_PORTAL:
			for (i = 0; i < arrays.numverts; i++)
			{
				// TODO
				color = Distance (r_eyepos, arrays.verts[i]) / 255.0f;
				color = bound (0.0f, color, 255.0f);
				col[i][3] = FloatToByte(color);
			}
			break;

		case ALPHA_GEN_VERTEX:
			if (pass->rgbgen != RGB_GEN_VERTEX)
			{
				for (i = 0; i < arrays.numverts; i++)
					col[i][3] = arrays.colour[i][3];
			}
			break;

		case ALPHA_GEN_ENTITY:
			for (i = 0; i < arrays.numverts; i++)
				col[i][3] = arrays.colour[i][3];
			break;

		// TODO
		// Looks good but it's incorrect. Uses mid value of 63 (127 was too bright)
		// Sum of all lights in the vertex origin (static + dyn)?
		case ALPHA_GEN_LIGHTINGSPECULAR:
			for (i = 0; i < arrays.numverts; i++)
				col[i][3] = 63;
			break;

		case ALPHA_GEN_DEFAULT:
		default:
			if ((pass->rgbgen != RGB_GEN_VERTEX) && (pass->rgbgen != RGB_GEN_IDENTITY))
			{
				for (i = 0; i < arrays.numverts; i++)
					col[i][3] = arrays.colour[i][3];
			}

			break;
	}

	return *(byte **)&col;
}

void R_Begin2d (void)
{
	GL_Enable (GL_SCISSOR_TEST);
	GL_Disable (GL_CULL_FACE);
	GL_DepthFunc (GL_ALWAYS);
	GL_DepthMask (GL_FALSE);
	GL_Enable (GL_BLEND);
	GL_BlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (r_overlay)
		return;

	glViewport(0, 0, winX, winY);
	glScissor(0, 0, winX, winY);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, winX, winY, 0, 0.000000, 1.000000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	r_overlay = atrue;
}

void R_End2d (void)
{
	GL_Disable (GL_BLEND);
	GL_DepthMask(GL_TRUE);
	GL_Enable (GL_CULL_FACE);
	GL_Disable (GL_SCISSOR_TEST);

	if (!r_overlay)
		return;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	r_overlay = afalse;
}

void R_BackendClear( void )
{
	arrays.numelems = 0;
	arrays.numverts = 0;
}

void R_DrawStretchPic (float x, float y, float w, float h, float s1, float t1, float s2, float t2, int hShader)
{
	quad_t q;
	extern colour_t r_actcolor;

	if (hShader < 0) 
		return;

	q.verts[0][0] = x;
	q.verts[0][1] = y;
	q.verts[0][2] = 0;
	q.tc[0][0] = s1;
	q.tc[0][1] = t1;
	Vector4Copy (r_actcolor, q.color[0]);
	
	q.verts[1][0] = x + w;
	q.verts[1][1] = y;
	q.verts[1][2] = 0;
	q.tc[1][0] = s2;
	q.tc[1][1] = t1;
	Vector4Copy (r_actcolor, q.color[1]);
	
	q.verts[2][0] = x + w;
	q.verts[2][1] = y + h;
	q.verts[2][2] = 0;
	q.tc[2][0] = s2;
	q.tc[2][1] = t2;
	Vector4Copy (r_actcolor, q.color[2]);
	
	q.verts[3][0] = x;
	q.verts[3][1] = y + h;
	q.verts[3][2] = 0;
	q.tc[3][0] = s1;
	q.tc[3][1] = t2;
	Vector4Copy (r_actcolor, q.color[3]);
	
	q.elems[0] = 0;
	q.elems[1] = 1;
	q.elems[2] = 2;
	q.elems[3] = 0;
	q.elems[4] = 2;
	q.elems[5] = 3;
	
	R_BackendPushQuad (&q);
	
	R_Begin2d ();

	R_BackendFlush_Generic (&r_shaders[hShader], 0);

	R_End2d ();
}

void DoGamma(void)
{
	float gamma = (bound (0.5f, r_gamma->value, 2.0f)) * 0.5;

	R_Begin2d();

	GL_Disable (GL_TEXTURE_2D);
	GL_BlendFunc (GL_DST_COLOR, GL_SRC_ALPHA);

	glColor4f (gamma, gamma, gamma, gamma);

	glBegin (GL_QUADS);
	glVertex2f (0,0);
	glVertex2f (winX, 0);
	glVertex2f (winX, winY);
	glVertex2f (0, winY);
	glEnd ();

	GL_Enable (GL_TEXTURE_2D);

	R_End2d();

	glColor4f (1.0, 1.0, 1.0, 1.0);
}

void R_BackendFlush (int shadernum, int lmtex)
{
	shader_t *s;

	if (shadernum < 0)
	{
		R_BackendClear();
		return;
	}

	s = &r_shaders[shadernum];

	switch (s->flush)
	{
		case SHADER_FLUSH_GENERIC:
			R_BackendFlush_Generic(s, lmtex);
			break;

		case SHADER_FLUSH_MULTITEXTURE_LIGHTMAP:
			R_BackendFlush_MultitextureLightmapped(s, lmtex);
			break;

		case SHADER_FLUSH_MULTITEXTURE_COMBINE:
			R_BackendFlush_MultitextureCombine(s, lmtex);
			break;

		case SHADER_FLUSH_VERTEX_LIT:
			R_BackendFlush_VertexLit(s, lmtex);
			break;

		default:
			break;
	}

	R_BackendClear();
}


static void R_BackendFlush_Generic( shader_t *s, int lmtex )
{
	shaderpass_t *pass;
	int i, texture;

	switch (s->cull)
	{
		case SHADER_CULL_DISABLE:
			GL_Disable (GL_CULL_FACE);
			break;
			
		case SHADER_CULL_FRONT:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_FRONT);
			break;
			
		case SHADER_CULL_BACK:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_BACK);
			break;
	}

	if (s->flags & SHADER_POLYGONOFFSET)
		GL_Enable (GL_POLYGON_OFFSET);
	else
		GL_Disable (GL_POLYGON_OFFSET);

	R_BackendMake_Vertices(s);

	glVertexPointer(3, GL_FLOAT, 0, arrays.verts);
	glNormalPointer(GL_FLOAT, 0, arrays.norms);

	if (glLockArraysEXT)
		glLockArraysEXT(0, arrays.numverts);

	if (gl_ext_info._ARB_Multitexture)
	{
		GL_ActiveTextureARB (GL_TEXTURE0_ARB);
		GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);
	}
	
	for (i = 0; i < s->numpasses; i++)
	{
		pass = &s->pass[i];
		
		// Set the Texture Coord Array
		glTexCoordPointer(2, GL_FLOAT, 0, R_BackendMake_TexCoords (pass, 0));
		
		// Set the Colors
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, R_BackendMake_Colors(pass));
		
		// Set the Texture
		if (pass->flags & SHADER_LIGHTMAP)
		{
			texture = r_lightmaps[lmtex];
		}
		else if (pass->flags & SHADER_ANIMMAP)
		{
			int frame;

			if (!pass->anim_numframes || pass->anim_numframes > SHADER_ANIM_FRAMES_MAX)
				return;

			frame = FloatToIntRet(shadertime * pass->anim_fps) % pass->anim_numframes;
			
			texture = pass->anim_frames[frame];
		}
		else {
			texture = pass->texref;
		}

		if (texture < 1)
			return;

		GL_BindTexture(GL_TEXTURE_2D, texture);	

		// Blending
		if (pass->flags & SHADER_BLEND)
		{
			GL_Enable (GL_BLEND);
			GL_BlendFunc(pass->blendsrc, pass->blenddst);	
		}
		else {
			GL_Disable(GL_BLEND);
		}

		// Alphafunc
		if (pass->flags & SHADER_ALPHAFUNC)
		{
			GL_Enable(GL_ALPHA_TEST);
			GL_AlphaFunc(pass->alphafunc, pass->alphafuncref);
		}
		else {
			GL_Disable(GL_ALPHA_TEST);
		}

		if (!r_overlay)
		{
			// Depth
			GL_DepthFunc(pass->depthfunc);

			if (pass->flags & SHADER_DEPTHWRITE)
				GL_DepthMask(GL_TRUE);
			else
				GL_DepthMask(GL_FALSE);
		}

		// Draw it
		glDrawElements(GL_TRIANGLES, arrays.numelems, GL_UNSIGNED_INT,
			arrays.elems);
	}

	if (glUnlockArraysEXT)
		glUnlockArraysEXT();
}

/*
================
R_BackendFlush_Multitexture_Lightmapped

Assumes a 2 pass shader. No blending
in first pass. Modulate int 2. pass.
================
*/
static void R_BackendFlush_MultitextureLightmapped (shader_t *s ,int lmtex )
{
	shaderpass_t *pass;
	int texture;

	if (s->numpasses != 2)
		return;

	R_BackendMake_Vertices (s);

	switch (s->cull)
	{
		case SHADER_CULL_DISABLE:
			GL_Disable (GL_CULL_FACE);
			break;

		case SHADER_CULL_FRONT:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_FRONT);
			break;

		case SHADER_CULL_BACK:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_BACK);
			break;
	}

	if (s->flags & SHADER_POLYGONOFFSET)
		GL_Enable (GL_POLYGON_OFFSET);
	else
		GL_Disable (GL_POLYGON_OFFSET);

	R_BackendMake_Vertices(s);

	glVertexPointer(3, GL_FLOAT, 0, arrays.verts);

	if (glLockArraysEXT)
		glLockArraysEXT(0, arrays.numverts);

	// Preparations
	GL_DisableClientState (GL_COLOR_ARRAY);

	// first pass
	pass = &s->pass[0];
	
	GL_ActiveTextureARB (GL_TEXTURE0_ARB);
	GL_Enable(GL_TEXTURE_2D);

	GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);
	GL_EnableClientState (GL_TEXTURE_COORD_ARRAY);
	
	GL_TexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	if (lmtex < 1)
		return;

	glTexCoordPointer(2, GL_FLOAT, 0, R_BackendMake_TexCoords(pass, 0));

	GL_BindTexture(GL_TEXTURE_2D, r_lightmaps[lmtex]);

	// second pass
	pass = &s->pass[1];

	GL_ActiveTextureARB (GL_TEXTURE1_ARB);
	GL_Enable(GL_TEXTURE_2D);

	GL_ClientActiveTextureARB (GL_TEXTURE1_ARB);
	GL_EnableClientState (GL_TEXTURE_COORD_ARRAY);

	GL_TexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glTexCoordPointer(2, GL_FLOAT, 0, R_BackendMake_TexCoords(pass, 1));

	if (pass->flags & SHADER_ANIMMAP)
	{
		int frame;

		if (!pass->anim_numframes || pass->anim_numframes > SHADER_ANIM_FRAMES_MAX) 
			return;

		frame = FloatToIntRet(shadertime * pass->anim_fps) % pass->anim_numframes;
		texture = pass->anim_frames[frame];
	}
	else
		texture = pass->texref;

	if (texture < 1)
		return;

	GL_BindTexture(GL_TEXTURE_2D, texture);

	// draw it
	glDrawElements(GL_TRIANGLES, arrays.numelems, GL_UNSIGNED_INT,
		arrays.elems);

	// cleanup
	GL_Disable(GL_TEXTURE_2D);
	GL_DisableClientState(GL_TEXTURE_COORD_ARRAY);

	GL_ActiveTextureARB(GL_TEXTURE0_ARB);
	GL_TexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);

	if (glUnlockArraysEXT)
		glUnlockArraysEXT();
}

/*
================
R_BackendFlush_Multitexture_Lightmapped

Assumes 2 passes. We could extend this. TODO!
================
*/
static void R_BackendFlush_MultitextureCombine (shader_t *s,int lmtex )
{
	if (s->numpasses != 2)
		return;

	R_BackendMake_Vertices (s);

	switch (s->cull)
	{
		case SHADER_CULL_DISABLE:
			GL_Disable (GL_CULL_FACE);
			break;

		case SHADER_CULL_FRONT:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_FRONT);
			break;

		case SHADER_CULL_BACK:
			GL_Enable (GL_CULL_FACE);
			GL_CullFace (GL_BACK);
			break;
	}

	if (s->flags & SHADER_POLYGONOFFSET)
		GL_Enable (GL_POLYGON_OFFSET);
	else
		GL_Disable (GL_POLYGON_OFFSET);

	R_BackendMake_Vertices(s);

	glVertexPointer(3, GL_FLOAT, 0, arrays.verts);

	if (glLockArraysEXT)
		glLockArraysEXT(0, arrays.numverts);
}

// TODO !!!
static void R_BackendFlush_VertexLit (shader_t *s, int lmtex )
{
}
