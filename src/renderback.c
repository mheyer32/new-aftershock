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


void Render_Backend_Flush (int shadernum, int lmtex);

static void Render_Backend_Flush_Generic (shader_t *s, int lmtex);
static void Render_Backend_Flush_Multitexture_Lightmapped (shader_t *s, int lmtex);
static void Render_Backend_Flush_Multitexture_Combine (shader_t *s, int lmtex);
static void Render_Backend_Flush_Vertex_Lit (shader_t *s, int lmtex);

static void render_pushface(cface_t *face);
static void render_pushmesh(mesh_t *mesh);
static void render_pushface_deformed(int shadernum, cface_t *face);
static void render_stripmine(int numelems, int *elems);
static int render_setstate(shaderpass_t *pass, uint_t lmtex);

arrays_t arrays;

aboolean r_overlay = afalse;

extern reference_t transform_ref;

__inline static double render_func_eval(uint_t func, float *args)
{
    // Evaluate a number of time based periodic functions
    double x  = (cl_frametime + args[2]) * args[3];
	x -= floor(x);

    switch (func)
    {
		case SHADER_FUNC_SIN:
			return sin(x * TWOPI) * args[1] + args[0];
			
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

void Render_Backend_Init(void)
{
    arrays.verts = (vec3_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
	arrays.norms = (vec3_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
    arrays.tex_st = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
	arrays.lm_st = (vec2_t *)malloc(MAX_ARRAYS_VERTS * sizeof(vec2_t));
	arrays.elems = (int *)malloc(MAX_ARRAYS_ELEMS * sizeof(int));
    arrays.colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.mod_colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.entity_colour = (colour_t *)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));	

	if (!r_allowExtensions->integer)
	{
		arrays.stage_tex_st = (vec2_t **)malloc (sizeof(vec2_t *));
		arrays.stage_tex_st[0] = (vec2_t *)malloc (MAX_ARRAYS_VERTS * sizeof(vec2_t));
		glconfig.maxActiveTextures = 1;
	}
	else 
	{
		int i;

		arrays.stage_tex_st = (vec2_t **)malloc (glconfig.maxActiveTextures * sizeof(vec2_t *));
		
		for (i = 0; i < glconfig.maxActiveTextures; i++)
			arrays.stage_tex_st[i] = (vec2_t *)malloc (MAX_ARRAYS_VERTS * sizeof(vec2_t));
	}
}

void Render_Backend_Shutdown(void)
{
	int i;

	free(arrays.verts);
    free(arrays.tex_st);
    free(arrays.lm_st);
	free(arrays.norms);
    free(arrays.elems);
    free(arrays.colour);
	free(arrays.mod_colour);

	for (i = 0; i < glconfig.maxActiveTextures; i++)
		free (arrays.stage_tex_st[i]);

	free (arrays.stage_tex_st);
}


void R_Push_Quad (quad_t *q)
{
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

void Render_Backend_Overlay (quad_t *q, int numquads)
{
	int i, shader = q->shader;
	quad_t *quad = &q[0];

	for (i = 0; i < numquads; i++, q++)
    {
		// Look for quads that share rendering state
		if (shader != quad->shader)
		{
			// Flush the renderer and reset
			Render_Backend_Flush(shader, 0);
			shader = quad->shader;
		}

		R_Push_Quad (quad);
    }

    // Final flush to clear queue
	Render_Backend_Flush(shader, 0);
}

void Render_Backend(facelist_t *facelist)
{
    int f, shader = 0, lmtex = 0;
    uint_t key = (uint_t)-1;
    cface_t *face;   

    arrays.numverts = arrays.numelems = 0;

    for (f = 0; f < facelist->numfaces; f++)
    {
		face = &cm.faces[facelist->faces[f].face];

		// Look for faces that share rendering state
		if (facelist->faces[f].sortkey != key)
		{
			// Flush the renderer and reset
			if (f) Render_Backend_Flush(shader, lmtex);
			shader = face->shadernum;
			lmtex = face->lightmapnum;
			key = facelist->faces[f].sortkey;
		}

		// Push the face to the triangle arrays
		switch (face->facetype)
		{
			case FACETYPE_NORMAL:
			case FACETYPE_TRISURF:
				render_pushface(face);
				break;

			case FACETYPE_MESH:
				render_pushmesh(&r_meshes[r_facemeshes[facelist->faces[f].face]]);
				break;

			default:
				break;
		}
    }

    // Final flush to clear queue
	Render_Backend_Flush(shader, lmtex);
}

void Render_Backend_Sky(int numsky, int *skylist)
{
    int s, i, shader = cm.faces[skylist[0]].shadernum;
    float skyheight = r_shaders[shader].skyheight;
    uint_t *elem;

    arrays.numverts = arrays.numelems = 0;

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

		for (i = 0; i < r_skybox->numpoints; i++)
		{
			VectorCopy(r_skybox->points[s][i], arrays.verts[arrays.numverts]);
			Vector2Copy(r_skybox->tex_st[s][i], arrays.tex_st[arrays.numverts]);
			arrays.numverts++;
		}
    }

	Render_Backend_Flush(shader, 0);
	
    // Restore world space
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


static void render_pushface(cface_t *face)
{
    int *elem = face->elems;
    cvertex_t *vert = face->verts;
	int i;

	if (arrays.numverts >= MAX_ARRAYS_VERTS)
		return;

    for (i = 0; i < face->numelems; i++)
		arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
    
    for (i = 0; i < face->numverts; i++)
	{
		VectorCopy(vert->v_point, arrays.verts[arrays.numverts]);
		VectorCopy(vert->v_norm, arrays.norms[arrays.numverts]);
		Vector2Copy(vert->tex_st, arrays.tex_st[arrays.numverts]);
		Vector2Copy(vert->lm_st, arrays.lm_st[arrays.numverts]);
		Vector4Copy(vert->colour, arrays.colour[arrays.numverts]);
		vert++;
		arrays.numverts++;
    }	    
}

void R_Push_raw (vec3_t *v, vec2_t *tc, colour_t *c, int *elems, int numverts, int numelems)
{
	int i;

	if (arrays.numverts >= MAX_ARRAYS_VERTS)
		return;

	for (i = 0; i < numelems; i++)
		arrays.elems[arrays.numelems++] = arrays.numverts + *elems++;

	for (i = 0; i < numverts; i++) 
	{
		VectorCopy(v[i], arrays.verts[arrays.numverts]);
		Vector2Copy(tc[i], arrays.tex_st[arrays.numverts]);
		Vector4Copy(c[i], arrays.colour[arrays.numverts]);
		arrays.numverts++;
	}
}

static void render_pushmesh(mesh_t *mesh)
{
    int  i, *elem = mesh->elems;

	if (arrays.numverts >= MAX_ARRAYS_VERTS)
		return;

	// any way to implement faceculling here in the engine ?

    for (i = 0; i < mesh->numelems; i++)
    {
		arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
    }
	
	for (i = 0; i < mesh->msize; i++)
	{
	    VectorCopy(mesh->points[i], arrays.verts[arrays.numverts]);
	    Vector2Copy(mesh->tex_st[i], arrays.tex_st[arrays.numverts]);
	    Vector2Copy(mesh->lm_st[i], arrays.lm_st[arrays.numverts]);
	    arrays.numverts++;
	}
}

static void render_stripmine(int numelems, int *elems)
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
void Render_Backend_Make_Vertices (shader_t *s)
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

float *Render_Backend_Make_TexCoords (shaderpass_t *pass, int stage)
{
	int i = arrays.numverts, n;
	vec2_t *in = arrays.tex_st;
	vec2_t *out = arrays.stage_tex_st[stage];
	vec3_t dir, pos;
	int j;
	float rot;
	float cost;
	float sint;

	switch (pass->tc_gen)
	{
		case TC_GEN_BASE:
			in = arrays.tex_st;
			break;
			
		case TC_GEN_LIGHTMAP:
			in = arrays.lm_st;
			break;
			
		// FIXME !!!
		// this is not cube-mapping!

		// TODO !!!
		case TC_GEN_ENVIRONMENT:
			in = arrays.stage_tex_st[stage];
			
			VectorCopy (r_eyepos, pos);

			if (!transform_ref.matrix_identity)
			{
				if (!transform_ref.inv_matrix_calculated)	
				{
					if (!Matrix4_Inverse(transform_ref.inv_matrix, transform_ref.matrix))
						Matrix4_Identity(transform_ref.inv_matrix);
					
					transform_ref.inv_matrix_calculated = atrue;	
				}
				
				Matrix_Multiply_Vec3(transform_ref.inv_matrix, pos, pos);
			}

			for (j = 0; j < arrays.numverts; j++)
			{
				VectorSubtract(pos, arrays.verts[j], dir);
				VectorNormalizeFast(dir);
															
				in[j][0] = dir[0] + arrays.norms[j][0];
				in[j][1] = dir[1] + arrays.norms[j][1];
			}

			break;
			
		case TC_GEN_VECTOR:		// Is this right ?
			in = arrays.stage_tex_st[stage];
				
			for (j = 0; j < arrays.numverts; j++)
			{
				in[j][0] = DotProduct(pass->tc_gen_s, arrays.verts[j]);
				in[j][1] = DotProduct(pass->tc_gen_t, arrays.verts[j]);
			}
			break;

		default:
			in = arrays.tex_st;
			break;
	}

	if (pass->num_tc_mod > 0)
	{
		float t1, t2, p1, p2;
		double pos;

		for (n = 0; n < pass->num_tc_mod; n++)
		{
			switch (pass->tc_mod[n].type)
			{
				case SHADER_TCMOD_ROTATE:
				{
					rot = pass->tc_mod[n].args[0] * cl_frametime * DEG2RAD;
					
					sint = (float)sin(rot);
					cost = (float)cos(rot);
					p1 = 0.5f * (1 - cost + sint);
					p2 = 0.5f * (1 - sint - cost);
					
					for (j = 0; j < arrays.numverts; j++)
					{
						out[j][0] = in[j][0] * cost - in[j][1] * sint + p1;
						out[j][1] = in[j][1] * cost + in[j][0] * sint + p2;
					}
				}
				break;
				
				case SHADER_TCMOD_SCALE:
				{
					for (j = 0; j < arrays.numverts; j++)
					{
						out[j][0] = in[j][0] * pass->tc_mod[n].args[0];
						out[j][1] =	in[j][1] * pass->tc_mod[n].args[1];
					}
				}
				break;
					
				case SHADER_TCMOD_TURB:
				{
					double k;

					pos = pass->tc_mod[n].args[2] + cl_frametime * pass->tc_mod[n].args[3];

					for (j = 0; j < arrays.numverts; j++)
					{
						k = (((arrays.verts[j][0] + arrays.verts[j][2]) / 1024.f + pos) * 1024.f) * DEG2RAD;
						out[j][0] = in[j][0] + pass->tc_mod[n].args[0] + pass->tc_mod[n].args[1] * sin(k);

						k = (((arrays.verts[j][1]) / 1024.f + pos) * 1024.f) * DEG2RAD;
						out[j][1] = in[j][1] + pass->tc_mod[n].args[0] + pass->tc_mod[n].args[1] * sin(k);
					}
				}
				
				break;
					
				case SHADER_TCMOD_STRETCH:
				{
					t1 = 1.0f / (float)render_func_eval(pass->tc_mod_stretch.func,
						pass->tc_mod_stretch.args);

					for (j = 0; j < arrays.numverts; j++) {
						out[j][0] = t1 * (in[j][0] - 0.5f) + 0.5f;
						out[j][1] = t1 * (in[j][1] - 0.5f) + 0.5f;
					}
				}

				break;
					
				case SHADER_TCMOD_SCROLL:
				{
					pos = pass->tc_mod[n].args[0] * cl_frametime;
					pos -= floor(pos);
					t1 = (float)pos;

					pos = pass->tc_mod[n].args[1] * cl_frametime;
					pos -= floor(pos);
					t2 = (float)pos;

					for (j = 0; j < arrays.numverts; j++) {
						out[j][0] = in[j][0] + t1;
						out[j][1] = in[j][1] + t2;
					}
				}
				
				break;
				
				case SHADER_TCMOD_TRANSFORM:
				{
					for (j = 0; j < arrays.numverts; j++) 
					{
						out[j][0] = in[j][0] * pass->tc_mod[n].args[0] + in[j][1] * pass->tc_mod[n].args[2] + pass->tc_mod[n].args[4];
						out[j][1] = in[j][1] * pass->tc_mod[n].args[1] + in[j][0] * pass->tc_mod[n].args[3] + pass->tc_mod[n].args[5];
					}
				}
				break;
	
				default:
					memcpy (out, in, arrays.numverts * sizeof(vec2_t));
					break;
			}
		}
	}
	else {
		memcpy (out, in, arrays.numverts * sizeof(vec2_t));
	}
	
	return *(float **)&out;
}

byte *Render_Backend_Make_Colors (shaderpass_t *pass)
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
			memset (arrays.mod_colour, 255, arrays.numverts * 4);
			col = arrays.mod_colour;
			break;

		case RGB_GEN_WAVE:
			c = FloatToByte(255.0f * (float)render_func_eval(pass->rgbgen_func.func,
							pass->rgbgen_func.args));
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
				arrays.mod_colour[i][2] = 255;
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
			memset (arrays.mod_colour, 255, arrays.numverts * 4);
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
	GL_Enable (GL_SCISSOR_TEST);
	GL_Disable (GL_CULL_FACE);
	GL_DepthFunc (GL_ALWAYS);
	GL_DepthMask (GL_FALSE);
	GL_Enable (GL_BLEND);
	GL_BlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	r_overlay = atrue;
}

void R_End2d (void)
{
	if (!r_overlay)
		return;

	GL_Disable (GL_BLEND);
	GL_DepthMask(GL_TRUE);
	GL_Enable (GL_CULL_FACE);
	GL_Disable (GL_SCISSOR_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	r_overlay = afalse;
}

void R_DrawStretchPic (float x, float y, float w, float h, float s1, float t1, float s2, float t2, int hShader)
{
	quad_t q;
	extern colour_t r_actcolor;

	if (hShader < 0) 
		return;

	arrays.numelems = arrays.numverts = 0;

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
	
	R_Push_Quad (&q);
	
	R_Begin2d ();

	Render_Backend_Flush_Generic (&r_shaders[hShader], 0);

	R_End2d ();
}

void Render_Backend_Flush (int shadernum, int lmtex)
{
	shader_t *s;

	if (shadernum < 0)
	{
		arrays.numverts = arrays.numelems = 0;
		return;
	}

	s = &r_shaders [shadernum];

	switch (s->flush)
	{
		case SHADER_FLUSH_GENERIC:
			Render_Backend_Flush_Generic(s, lmtex);
			break;

		case SHADER_FLUSH_MULTITEXTURE_LIGHTMAP:
			Render_Backend_Flush_Multitexture_Lightmapped(s, lmtex);
			break;

		case SHADER_FLUSH_MULTITEXTURE_COMBINE:
			Render_Backend_Flush_Multitexture_Combine(s, lmtex);
			break;

		case SHADER_FLUSH_VERTEX_LIT:
			Render_Backend_Flush_Vertex_Lit(s, lmtex);
			break;

		default:
			break;
	}

	arrays.numverts = arrays.numelems = 0;
}


static void Render_Backend_Flush_Generic (shader_t *s ,int lmtex )
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

	Render_Backend_Make_Vertices(s);

	glVertexPointer(3, GL_FLOAT, 0, arrays.verts);

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
		glTexCoordPointer(2, GL_FLOAT, 0, Render_Backend_Make_TexCoords (pass, 0));
		
		// Set the Colors
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, Render_Backend_Make_Colors(pass));
		
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

			frame = (int)(cl_frametime * pass->anim_fps) % pass->anim_numframes;
			
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
Render_Backend_Flush_Multitexture_Lightmapped

Assumes a 2 pass shader. No blending
in first pass. Modulate int 2. pass.
================
*/
static void Render_Backend_Flush_Multitexture_Lightmapped (shader_t *s ,int lmtex )
{
	shaderpass_t *pass;
	int texture;

	if (s->numpasses != 2)
		return;

	Render_Backend_Make_Vertices (s);

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

	Render_Backend_Make_Vertices(s);

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

	glTexCoordPointer(2, GL_FLOAT, 0, Render_Backend_Make_TexCoords(pass, 0));

	GL_BindTexture(GL_TEXTURE_2D, r_lightmaps[lmtex]);

	// second pass
	pass = &s->pass[1];

	GL_ActiveTextureARB (GL_TEXTURE1_ARB);
	GL_Enable(GL_TEXTURE_2D);

	GL_ClientActiveTextureARB (GL_TEXTURE1_ARB);
	GL_EnableClientState (GL_TEXTURE_COORD_ARRAY);

	GL_TexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glTexCoordPointer(2, GL_FLOAT, 0, Render_Backend_Make_TexCoords(pass, 1));

	if (pass->flags & SHADER_ANIMMAP)
	{
		int frame;

		if (!pass->anim_numframes || pass->anim_numframes > SHADER_ANIM_FRAMES_MAX) 
			return;

		frame = (int)(cl_frametime * pass->anim_fps) % pass->anim_numframes;
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
Render_Backend_Flush_Multitexture_Lightmapped

Assumes 2 passes. We could extend this. TODO!
================
*/
static void Render_Backend_Flush_Multitexture_Combine (shader_t *s,int lmtex )
{
	if (s->numpasses != 2)
		return;

	Render_Backend_Make_Vertices (s);

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

	Render_Backend_Make_Vertices(s);

	glVertexPointer(3, GL_FLOAT, 0, arrays.verts);

	if (glLockArraysEXT)
		glLockArraysEXT(0, arrays.numverts);
}

// TODO !!!
static void Render_Backend_Flush_Vertex_Lit (shader_t *s, int lmtex )
{
}
