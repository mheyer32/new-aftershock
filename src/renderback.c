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
#include "util.h"
#include "bsp.h"
#include "shader.h"
#include "render.h"
#include "tex.h"
#include "lightmap.h"
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



void Render_Backend_Flush (int shadernum ,int lmtex );

static void Render_Backend_Flush_Generic (shader_t *s ,int lmtex );
static void Render_Backend_Flush_Multitexture_Lightmapped (shader_t *s ,int lmtex );
static void Render_Backend_Flush_Multitexture_Combine (shader_t *s,int lmtex );
static void Render_Backend_Flush_Vertex_Lit (shader_t *s, int lmtex );


static void render_pushface(cface_t *face);
static void render_pushmesh(mesh_t *mesh);
static double render_func_eval(uint_t func, float *args);
static void render_pushface_deformed(int shadernum, cface_t *face);
static void render_stripmine(int numelems, int *elems);
static int render_setstate(shaderpass_t *pass, uint_t lmtex);
static void render_pushmesh_deformed(mesh_t *mesh,cface_t *face);

static colour_t * R_Make_Rgba (shaderpass_t * pass );
static float * R_Make_TexCoords (shaderpass_t * pass ,int stage);
static void R_Make_Vertices (shader_t *s );
arrays_t arrays;



extern reference_t transform_ref;

void
R_backend_init(void)
{
    
    arrays.verts = (vec3_t*)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
	arrays.norms = (vec3_t*)malloc(MAX_ARRAYS_VERTS * sizeof(vec3_t));
    arrays.tex_st = (texcoord_t*)malloc(MAX_ARRAYS_VERTS * sizeof(texcoord_t));
	arrays.lm_st = (texcoord_t*)malloc(MAX_ARRAYS_VERTS * sizeof(texcoord_t));
	
	arrays.elems = (int*)malloc(MAX_ARRAYS_ELEMS * sizeof(int));
    arrays.colour = (colour_t*)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.mod_colour= (colour_t*)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));
	arrays.entity_colour = 	(colour_t*)malloc(MAX_ARRAYS_VERTS * sizeof(colour_t));	

	if (!r_allowExtensions->integer)
	{

		arrays.stage_tex_st = (vec2_t ** ) malloc (1 * sizeof(vec2_t*));
		arrays.stage_tex_st[0] =  (vec2_t * ) malloc (MAX_ARRAYS_VERTS * sizeof(vec2_t));
	}
	else 
	{
		int i;
		arrays.stage_tex_st = (vec2_t ** ) malloc (glconfig.maxActiveTextures * sizeof(vec2_t*));
		
		for (i=0;i<glconfig.maxActiveTextures;i++)
		{	
			arrays.stage_tex_st[i] =  (vec2_t * ) malloc (MAX_ARRAYS_VERTS * sizeof(vec2_t));
		}

	}



}

void R_backend_shutdown(void )
{

	free(arrays.verts);
    free(arrays.tex_st);
    free(arrays.lm_st);
	free (arrays.norms);
    free(arrays.elems);
    free(arrays.colour);
	free (arrays.mod_colour);


	if (!r_allowExtensions->integer)
	{
		free (arrays.stage_tex_st[0]);
	}
	else 
	{
		int i;
		for (i=0;i<glconfig.maxActiveTextures;i++)
		{
			free (arrays.stage_tex_st[i]);

		}

	}

	free (arrays.stage_tex_st);


}


static void R_Push_Quad ( quad_t * q )
{

	int i;
	//int * elem=elems;

	for (i=0;i<6;i++)
	{
	arrays.elems[arrays.numelems++] = arrays.numverts + q->elems[i];
	}


	for (i=0;i<4;i++)
	{
		vec_copy(q->verts[i], arrays.verts[arrays.numverts]);
		vec2_copy(q->tc[i], arrays.tex_st[arrays.numverts]);
		colour_copy(q->color[i], arrays.colour[arrays.numverts]);

		arrays.numverts++;
	}

	
}


colour_t * R_Make_Rgba (shaderpass_t * pass )
{
	int i;
	byte rgb;
	colour_t * col;
	switch (pass->rgbgen)
	{
	case RGB_GEN_NONE:  // should not happen !!
		break;

	case RGB_GEN_IDENTITY_LIGHTING :
		memset (arrays.mod_colour,r_overBrightBits->integer ? 128 : 255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;
	case RGB_GEN_IDENTITY:
		memset (arrays.mod_colour,255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;
	case RGB_GEN_WAVE :
		rgb = (byte)255 * (float)render_func_eval(pass->rgbgen_func.func,
					    pass->rgbgen_func.args);
		memset (arrays.mod_colour,rgb,arrays.numverts * 4 );
		for (i=0;i< arrays.numverts;i++)
			arrays.mod_colour[i][3] =255;

		col= arrays.mod_colour;
		break;
	case RGB_GEN_ENTITY :
		col= arrays.entity_colour;
		break;
	case RGB_GEN_ONE_MINUS_ENTITY:
		for (i=0;i<arrays.numverts;i++)
		{
			arrays.mod_colour[i][0] = 255 - arrays.entity_colour[i][0];
			arrays.mod_colour[i][1] = 255 - arrays.entity_colour[i][1];
			arrays.mod_colour[i][2] = 255 - arrays.entity_colour[i][2];
			arrays.mod_colour[i][2] = 255 ;
		}
		break;
	case RGB_GEN_VERTEX:
		col= arrays.colour;
		break;
	case RGB_GEN_ONE_MINUS_VERTEX:
		for (i=0;i<arrays.numverts;i++)
		{
			arrays.mod_colour[i][0] = 255 - arrays.colour[i][0];
			arrays.mod_colour[i][1] = 255 - arrays.colour[i][1];
			arrays.mod_colour[i][2] = 255 - arrays.colour[i][2];
		}
		col= arrays.mod_colour;
		break;
	case RGB_GEN_LIGHTING_DIFFUSE :
		// TODO 
		memset (arrays.mod_colour,255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;




	default :
		col= arrays.colour;

		break;

	}


		// TODO !!!!
	switch (pass->alpha_gen)
	{

	case  ALPHA_GEN_PORTAL :
		
		for (i=0;i<arrays.numverts;i++)
		{
			// TODO 
			int alpha;
			alpha = 255.0 * 1.0 /Distance (r_eyepos,arrays.verts[i]);	
			
			if (alpha>255 )
				alpha = 255;

			col[i][3] = alpha;
		}
		// TODO
	case ALPHA_GEN_DEFAULT :
	case ALPHA_GEN_VERTEX :
	case ALPHA_GEN_ENTITY:
	case ALPHA_GEN_LIGHTINGSPECULAR:
	default :
		for (i=0;i<arrays.numverts;i++)
			col[i][3]=arrays.colour[i][3];
		
	break;

	}


	return col;

}






void Render_backend_Overlay ( quad_t * q,int numquads )
{

	int i,shader=q->shader;
	quad_t * quad;


	for (i=0; i < numquads; ++i)
    {
		quad=&q[i];

	/* Look for faces that share rendering state */
	if (shader != quad->shader)
	{
	    /* Flush the renderer and reset */
	  
			Render_Backend_Flush(shader, 0);
	
	    shader = quad->shader;
	}

	R_Push_Quad (quad);




    }
    /* Final flush to clear queue */
	Render_Backend_Flush(shader, 0);

}

void
render_backend(facelist_t *facelist)
{
    int f, shader, lmtex;
    uint_t key;
    cface_t *face;   
    GL_EnableClientState(GL_VERTEX_ARRAY);
    GL_EnableClientState(GL_TEXTURE_COORD_ARRAY);

    arrays.numverts = arrays.numelems = 0;


    key = (uint_t)-1;
	
    for (f=0; f < facelist->numfaces; ++f)
    {
	face = &map.faces[facelist->faces[f].face];

	/* Look for faces that share rendering state */
	if (facelist->faces[f].sortkey != key)
	{
	    /* Flush the renderer and reset */
	    if (f)
		Render_Backend_Flush(shader,lmtex);
	    shader = face->shadernum;
	    lmtex = face->lm_texnum;
	    key = facelist->faces[f].sortkey;
	}

	/* Push the face to the triangle arrays */
	switch (face->facetype)
	{
	    case FACETYPE_NORMAL:
	    case FACETYPE_TRISURF:
		    render_pushface(face);
		break;
	    case FACETYPE_MESH:
		if (facelist->faces[f].face<r_nummeshes)// bugfix : need to find the source !!!
		render_pushmesh(&r_meshes[facelist->faces[f].face]);
		break;
	    default:
		break;
	}
    }
    /* Final flush to clear queue */
	
	Render_Backend_Flush(shader,lmtex);


	
   
}

void
render_backend_sky(int numsky, int *skylist)
{
    int s, i, shader;
    float skyheight;
    uint_t *elem;

    shader = map.faces[skylist[0]].shadernum;
    skyheight = r_shaders[shader].skyheight;
    arrays.numverts = arrays.numelems = 0;

   // glEnableClientState(GL_VERTEX_ARRAY);
   // glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    /* Center skybox on camera to give the illusion of a larger space */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(r_eyepos[0], r_eyepos[1], r_eyepos[2]);
    glScalef(skyheight, skyheight, skyheight);

    /* FIXME: Need to cull skybox based on face list */
    for (s=0; s < 5; s++)
    {
	elem = r_skybox->elems;
	for (i=0; i < r_skybox->numelems; i++)
	{
	    arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	}
	for (i=0; i < r_skybox->numpoints; i++)
	{
	    vec_copy(r_skybox->points[s][i], arrays.verts[arrays.numverts]);
	    vec2_copy(r_skybox->tex_st[s][i], arrays.tex_st[arrays.numverts]);
	    arrays.numverts++;
	}
    }
	Render_Backend_Flush(shader,0);
	
    /* Restore world space */
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


static void
render_pushface(cface_t *face)
{
    int  *elem,*finaladdress;
    vertex_t *vert;
    elem = face->elems;
	finaladdress=elem+face->numelems;
	while (elem<finaladdress)
	{
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
    }
    
    vert = face->verts;
	finaladdress=(void *)(vert+face->numverts);
	while (vert<(vertex_t *)finaladdress)
	{
	vec_copy(vert->v_point, arrays.verts[arrays.numverts]);
	VectorCopy (vert->v_norm ,arrays.norms [arrays.numverts]);

	vec2_copy(vert->tex_st, arrays.tex_st[arrays.numverts]);
	vec2_copy(vert->lm_st, arrays.lm_st[arrays.numverts]);
	colour_copy(vert->colour, arrays.colour[arrays.numverts]);
	vert++;
	arrays.numverts++;
    }	    
}



void R_Push_raw (vec3_t * v,vec2_t *tc ,colour_t *c,int * elems ,int numverts ,int numelems )
{
	int i;
	//int * elem=elems;

	for (i=0;i<numelems;i++)
	{
	arrays.elems[arrays.numelems++] = arrays.numverts + *elems++;
	}


	for (i=0;i<numverts;i++)
	{
		vec_copy(v[i], arrays.verts[arrays.numverts]);
		vec2_copy(tc[i], arrays.tex_st[arrays.numverts]);
		colour_copy(c[i], arrays.colour[arrays.numverts]);

		arrays.numverts++;
	}


}


static void
render_pushmesh(mesh_t *mesh)
{
    int  i,*elem;

	// any way to implement faceculling here in the engine ?

	// shit happens 
	if (mesh->size[0]>10000 || mesh->size[0]>10000 || mesh->size[0]<0 || mesh->size[1]<0 ) return;
    elem = mesh->elems;
    for (i = 0; i < mesh->numelems/3; ++i)
    {
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
    }
	

	for (i=0;i<mesh->size[1]*mesh->size[0];i++)
	{

		

	    vec_copy(mesh->points[i], arrays.verts[arrays.numverts]);
	    //arrays.verts[arrays.numverts][3] = 1.0f;
	    vec2_copy(mesh->tex_st[i],  arrays.tex_st[arrays.numverts]);
	    vec2_copy(mesh->lm_st[i], arrays.lm_st[arrays.numverts]);
	    arrays.numverts++;
	   
	}
  
}

static void
render_stripmine(int numelems, int *elems)
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

static double
render_func_eval(uint_t func, float *args)
{
    double x, y;

    /* Evaluate a number of time based periodic functions */
    /* y = args[0] + args[1] * func( (time + arg[3]) * arg[2] ) */
    
    x = (g_frametime + args[2]) * args[3];
    x -= floor(x);

    switch (func)
    {
	case SHADER_FUNC_SIN:
	    y = sin(x * TWOPI);
	    break;
	    
	case SHADER_FUNC_TRIANGLE:
	    if (x < 0.5)
		y = 2.0 * x - 1.0;
	    else
		y = -2.0 * x + 2.0;
	    break;
	    
	case SHADER_FUNC_SQUARE:
	    if (x < 0.5)
		y = 1.0;
	    else
		y = -1.0;
	    break;
	    
	case SHADER_FUNC_SAWTOOTH:
	    y = x;
	    break;
	    
	case SHADER_FUNC_INVERSESAWTOOTH:
	    y = 1.0 - x;
	    break;
    }

    return y * args[1] + args[0];
}
/*
static int
render_setstate(shaderpass_t *pass, uint_t lmtex)
{// TODO !!!!!
    if (pass->flags & SHADER_LIGHTMAP)
    {
	/* Select lightmap texture */
/*	GL_BindTexture(GL_TEXTURE_2D, map.lightmaps[lmtex]);
    }
    else if (pass->flags & SHADER_ANIMMAP)
    {
	uint_t texobj;
	int frame;

	/* Animation: get frame for current time */
/*	if (!pass->anim_numframes || pass->anim_numframes > 8) return 0;
	frame = (int)(g_frametime * pass->anim_fps) % pass->anim_numframes;
	texobj = pass->anim_frames[frame];
	if (texobj < 0) return 0;
	GL_BindTexture(GL_TEXTURE_2D, texobj);
    }
    else
    {
	uint_t texobj;

	/* Set normal texture */
/*	if (pass->texref < 0) return 0;
	texobj = pass->texref;
	GL_BindTexture(GL_TEXTURE_2D, texobj);
    }

	glTexCoordPointer(2, GL_FLOAT, 0, R_Make_TexCoords(pass,0));

    if (pass->flags & SHADER_BLEND)
    {
	GL_Enable(GL_BLEND);
	GL_BlendFunc(pass->blendsrc, pass->blenddst);
    }
    else
	GL_Disable(GL_BLEND);

    if (pass->flags & SHADER_ALPHAFUNC)
    {
	GL_Enable(GL_ALPHA_TEST);
	GL_AlphaFunc(pass->alphafunc, pass->alphafuncref);
    }
    else
	GL_Disable(GL_ALPHA_TEST);
    
    GL_DepthFunc(pass->depthfunc);
    if (pass->flags & SHADER_DEPTHWRITE)
	GL_DepthMask(GL_TRUE);
    else
	GL_DepthMask(GL_FALSE);

    GL_EnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, R_Make_Rgba (pass));
  
    return 1;
}
*/

// TODO !!!
static void Render_Backend_Make_Vertices (shader_t *s )
{
	int i;

	if (s->flags & SHADER_DEFORMVERTS )
	{
		float deflect ;
		vec3_t v;
		switch (s->deform_vertices)
		{

		case DEFORMV_NONE:
			break;
		case DEFORMV_WAVE:

			for (i=0;i<arrays.numverts;i++)
			{
			deflect = render_func_eval(s->deformv_wavefunc.func, s->deformv_wavefunc.args);
			deflect *= s->deform_params[0];
			VectorCopy(arrays.norms[i], v);
			VectorScale(v, deflect, v);
			VectorAdd(v, arrays.verts[i], arrays.verts[i]);
			}
			break;
		case DEFORMV_NORMAL:
			break;
		case DEFORMV_BULGE:
			break;
		case DEFORMV_MOVE:
			break;
		case DEFORMV_AUTOSPRITE:
			break;
		case DEFORMV_AUTOSPRITE2:
			break;



		default :
			break;
		}

	}


}

static float * Render_Backend_Make_TexCoords (shaderpass_t * pass ,int stage )
{

	int i=arrays.numverts,n;
	mat4_t mat,mat2; 
	vec2_t * in = arrays.tex_st;
	vec2_t * out ;

	Matrix4_Identity(mat);


	switch (pass->tc_gen)
	{
	case TC_GEN_BASE :
		in =arrays.tex_st;
		break;
	case TC_GEN_LIGHTMAP:
		in = arrays.lm_st;

	case TC_GEN_ENVIRONMENT:
		{
			// TODO !!!
		vec3_t pos ,v,n;
		vec3_t dir;
		int j;
		in= arrays.stage_tex_st[stage];

		
		// FIXME !!!
		// this is not cube-mapping !
		VectorCopy (r_eyepos ,pos );
		if (!transform_ref.matrix_identity)
		{
			if (!transform_ref.inv_matrix_calculated)	
			{
				if (!Matrix4_Inverse(transform_ref.inv_matrix,transform_ref.matrix))
					Matrix4_Identity(transform_ref.inv_matrix);

				transform_ref.inv_matrix_calculated=atrue;	
			}
			Matrix_Multiply_Vec3(transform_ref.inv_matrix,pos,pos);
		}

		for(j=0; j<arrays.numverts; j++)
			{
			
				VectorCopy (arrays.verts[j],v);
			
				VectorSubtract(v, pos, dir);
				VectorNormalize(dir);

				VectorCopy (arrays.norms[j],n);

				dir[0]+=n[0];
				dir[1]+=n[1];

				in[j][0]=dir[0];
				in[j][1]=dir[1];
			}
			
			
		}

		break;

	case TC_GEN_VECTOR :
		{
			int j;
			// Is this right ?
			in=arrays.stage_tex_st[stage];

			for (j=0;j<arrays.numverts;j++)
			{
				in[j][0]=DotProduct(pass->tc_gen_s,arrays.verts[j]);
				in[j][1]=DotProduct(pass->tc_gen_t,arrays.verts[j]);
			}
			break;
		}
	default :
		in=arrays.tex_st;
	}

	if (pass->num_tc_mod >0)
	{
		Matrix4_Identity ( mat2);

		out=arrays.stage_tex_st[stage];

		mat2[12] =0.5f;
		mat2[13] =0.5f;

		Matrix4_Multiply (mat,mat2,mat);
		
		for (n=0;n<pass->num_tc_mod;n++)
		{
			switch (pass->tc_mod[n].type)
			{
				case SHADER_TCMOD_ROTATE:
				{
					float rot=pass->tc_mod[n].args[0] * g_frametime * DEG2RAD ;
					float cost=cos(rot);
					float sint=sin(rot);

					Matrix4_Identity (mat2);
					mat2[0] = cost;
					mat2[1] = sint;
					mat2[4] = -sint;
					mat2[5] = cost;
			

					Matrix4_Multiply  (mat,mat2,mat);
				
				}
				break;
	
				case SHADER_TCMOD_SCALE:
				{
					Matrix4_Identity (mat2);
					mat2[0] =pass->tc_mod[n].args[0];
					mat2[5] = pass->tc_mod[n].args[1];
			
					Matrix4_Multiply  (mat,mat2,mat);
				}
				break;
				case SHADER_TCMOD_TURB:
				{
					// TODO 
					float scale[2];
					double x, y1, y2;
					x = (g_frametime + pass->tc_mod[n].args[2]) * pass->tc_mod[n].args[3];
					x -= floor(x);
					y1 = sin(x * TWOPI) * pass->tc_mod[n].args[1] + pass->tc_mod[n].args[0];
					y2 = sin((x+0.25) * TWOPI) * pass->tc_mod[n].args[1] +
					pass->tc_mod[n].args[0];
	    

					scale[0]= 1.0+y1*TURB_SCALE ;
					scale[1]= 1.0+y2*TURB_SCALE;

			
					Matrix4_Identity (mat2);
					mat2[0] =scale[0];
					mat2[5] = scale[1];
			
					Matrix4_Multiply  (mat,mat2,mat);
				}
				break;

				case SHADER_TCMOD_STRETCH:
				{
	     
					float factor=1.0f/(float)render_func_eval(pass->tc_mod_stretch.func,
					pass->tc_mod_stretch.args);

					Matrix4_Identity (mat2);
					mat2[0] =factor;
					mat2[5] = factor;
			
					Matrix4_Multiply  (mat,mat2,mat);
		
				}
				break;
				case SHADER_TCMOD_SCROLL:
				{
					vec2_t scroll;
					scroll[0] = pass->tc_mod[n].args[0] * g_frametime;
					scroll[1] = pass->tc_mod[n].args[1] * g_frametime;
			

					Matrix4_Identity (mat2);

					mat2[12] = scroll[0];
					mat2[13] = scroll[1];

					Matrix4_Multiply  (mat,mat2,mat);
				}
				break;
				case SHADER_TCMOD_NONE:
				default:
					break;
			}
		}

		Matrix4_Identity ( mat2);
		mat2[12] =-0.5f;
		mat2[13] =-0.5f;

		Matrix4_Multiply  (mat,mat2,mat);

		do {
			Matrix_Multiply_Vec2(mat,*in++,*out++);
		}while ( --i );

		out=arrays.stage_tex_st[stage];
	}
	
	else 
	{
		out=arrays.stage_tex_st[stage];
		memcpy (out,in,arrays.numverts * sizeof ( vec2_t ));
	}

	return *(float**)&out;


}

static byte * Render_Backend_Make_Colors ( shaderpass_t * pass )
{
	int i;
	byte rgb;
	colour_t * col;


	switch (pass->rgbgen)
	{

	case RGB_GEN_IDENTITY_LIGHTING :
		memset (arrays.mod_colour,r_overBrightBits->integer ? 128 : 255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;
	case RGB_GEN_IDENTITY:
		memset (arrays.mod_colour,255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;
	case RGB_GEN_WAVE :
		rgb = (byte)255 * (float)render_func_eval(pass->rgbgen_func.func,
					    pass->rgbgen_func.args);
		memset (arrays.mod_colour,rgb,arrays.numverts * 4 );
		for (i=0;i< arrays.numverts;i++)
			arrays.mod_colour[i][3] =255;

		col= arrays.mod_colour;
		break;
	case RGB_GEN_ENTITY :
		col= arrays.entity_colour;
		break;
	case RGB_GEN_ONE_MINUS_ENTITY:
		for (i=0;i<arrays.numverts;i++)
		{
			arrays.mod_colour[i][0] = 255 - arrays.entity_colour[i][0];
			arrays.mod_colour[i][1] = 255 - arrays.entity_colour[i][1];
			arrays.mod_colour[i][2] = 255 - arrays.entity_colour[i][2];
			arrays.mod_colour[i][2] = 255 ;
		}
		break;
	case RGB_GEN_VERTEX:
		col= arrays.colour;
		break;
	case RGB_GEN_ONE_MINUS_VERTEX:
		for (i=0;i<arrays.numverts;i++)
		{
			arrays.mod_colour[i][0] = 255 - arrays.colour[i][0];
			arrays.mod_colour[i][1] = 255 - arrays.colour[i][1];
			arrays.mod_colour[i][2] = 255 - arrays.colour[i][2];
		}
		col= arrays.mod_colour;
		break;
	case RGB_GEN_LIGHTING_DIFFUSE :
		// TODO 
		memset (arrays.mod_colour,255 ,arrays.numverts *4 );
		col= arrays.mod_colour;
		break;




	default :
		col= arrays.colour;

		break;

	}


		// TODO !!!!
	switch (pass->alpha_gen)
	{

	case  ALPHA_GEN_PORTAL :
		
		for (i=0;i<arrays.numverts;i++)
		{
			// TODO 
			int alpha;
			alpha = 255.0 * 1.0 /Distance (r_eyepos,arrays.verts[i]);	
			
			if (alpha>255 )
				alpha = 255;

			col[i][3] = alpha;
		}
		// TODO
	case ALPHA_GEN_DEFAULT :
	case ALPHA_GEN_VERTEX :
	case ALPHA_GEN_ENTITY:
	case ALPHA_GEN_LIGHTINGSPECULAR:
	default :
		for (i=0;i<arrays.numverts;i++)
			col[i][3]=arrays.colour[i][3];
		
	break;

	}


	return *(byte**)&col;

}


void Render_Backend_Flush (int shadernum ,int lmtex )
{
	shader_t *s;

	if (shadernum < 0)
	{
		arrays.numverts=arrays.numelems=0;
		return ;
	}

	s=&r_shaders [shadernum];

	switch (s->flush)
	{
	case SHADER_FLUSH_GENERIC:
		Render_Backend_Flush_Generic(s,lmtex);
		break;
	case SHADER_FLUSH_MULTITEXTURE_LIGHTMAP:
		Render_Backend_Flush_Multitexture_Lightmapped(s,lmtex);
		break;
	case SHADER_FLUSH_MULTITEXTURE_COMBINE :
		Render_Backend_Flush_Multitexture_Combine(s,lmtex);
		break;
	case SHADER_FLUSH_VERTEX_LIT:
		Render_Backend_Flush_Vertex_Lit(s,lmtex);
		break;
	default :
		arrays.numverts=arrays.numelems=0;
		return ;
	}


}


static void Render_Backend_Flush_Generic (shader_t *s ,int lmtex )
{

	shaderpass_t *pass;
	int i,texture;


	switch (s->cull )
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
		glLockArraysEXT(0,arrays.numverts);

	if (gl_ext_info._ARB_Multitexture)
	{
		GL_ActiveTextureARB (GL_TEXTURE0_ARB );
		GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);
	}
	
	
	for (i=0;i<s->numpasses;i++)
	{
		pass=&s->pass[i];
		
		// Set the Texture Coord Array :
		glTexCoordPointer(2, GL_FLOAT, 0,Render_Backend_Make_TexCoords (pass,0 ));
		
		// Set the Colors :
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, Render_Backend_Make_Colors(pass));
		

		// Set the Texture  :
		if (pass->flags & SHADER_LIGHTMAP )
		{
			texture=lmtex;
		}
		else if (pass->flags & SHADER_ANIMMAP )
		{
			int frame ;

			if (!pass->anim_numframes || pass->anim_numframes > 8) return ;
			frame = (int)(g_frametime * pass->anim_fps) % pass->anim_numframes;
			
			texture =pass->anim_frames[frame];
		}
		else
		{
			texture =pass->texref;
		}

		if ( !texture || texture< 0 )
		{
			arrays.numverts=arrays.numelems=0;
			return ;
		}

		GL_BindTexture(GL_TEXTURE_2D, texture);	

		// Blending :
		if (pass->flags & SHADER_BLEND )
		{
			GL_Enable (GL_BLEND );
			GL_BlendFunc(pass->blendsrc, pass->blenddst);	
		}
		else 
			GL_Disable(GL_BLEND);

		// Alphafunc :
		if (pass->flags & SHADER_ALPHAFUNC)
		{
			GL_Enable(GL_ALPHA_TEST);
			GL_AlphaFunc(pass->alphafunc, pass->alphafuncref);
		}
		else
			GL_Disable(GL_ALPHA_TEST);

		// Depth :
		GL_DepthFunc(pass->depthfunc);
		if (pass->flags & SHADER_DEPTHWRITE)
			GL_DepthMask(GL_TRUE);
		else
			GL_DepthMask(GL_FALSE);


		// Draw it :
		glDrawElements(GL_TRIANGLES, arrays.numelems, GL_UNSIGNED_INT,
		       arrays.elems);

	}

	
	if (glUnlockArraysEXT)
		glUnlockArraysEXT();


	arrays.numverts=arrays.numelems=0;

}

// assumes a 2 pass shader 
// no blending in first pass 
// Modulate int 2. pass

static void Render_Backend_Flush_Multitexture_Lightmapped (shader_t *s ,int lmtex )
{
	shaderpass_t * pass;
	int texture ;


	if (s->numpasses != 2 )
		return ;

	Render_Backend_Make_Vertices (s);

	
	switch (s->cull )
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
		glLockArraysEXT(0,arrays.numverts);


	// Preparations :

	GL_DisableClientState (GL_COLOR_ARRAY);
	

	// FIRST PASS : 

	pass = &s->pass[0];
	
	GL_ActiveTextureARB (GL_TEXTURE0_ARB );
	GL_Enable(GL_TEXTURE_2D);

	GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);
	GL_EnableClientState (GL_TEXTURE_COORD_ARRAY );
	
	GL_TexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	
	if (!lmtex || lmtex < 0)
	{
		arrays.numverts=arrays.numelems=0;
		return ;
	}

	glTexCoordPointer(2, GL_FLOAT, 0,Render_Backend_Make_TexCoords(pass,0));

	GL_BindTexture(GL_TEXTURE_2D,lmtex);

	// SECOND PASS :
	pass = &s->pass[1];

	GL_ActiveTextureARB (GL_TEXTURE1_ARB );
	GL_Enable(GL_TEXTURE_2D );

	GL_ClientActiveTextureARB (GL_TEXTURE1_ARB);
	GL_EnableClientState (GL_TEXTURE_COORD_ARRAY );

	GL_TexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glTexCoordPointer(2, GL_FLOAT, 0,Render_Backend_Make_TexCoords(pass,1));

	if (pass->flags & SHADER_ANIMMAP )
	{
		int frame ;

		if (!pass->anim_numframes || pass->anim_numframes > 8) return ;
		frame = (int)(g_frametime * pass->anim_fps) % pass->anim_numframes;
			
		texture =pass->anim_frames[frame];
	}
	else
	{
		texture =pass->texref;
	}

	if (!texture || texture<0 )
	{
		arrays.numverts=arrays.numelems=0;
		return ;
	}

	GL_BindTexture(GL_TEXTURE_2D,texture);


	// Draw it :
		glDrawElements(GL_TRIANGLES, arrays.numelems, GL_UNSIGNED_INT,
		       arrays.elems);


	// Cleanup :
	GL_Disable(GL_TEXTURE_2D );
	GL_DisableClientState (GL_TEXTURE_COORD_ARRAY );

	GL_ActiveTextureARB (GL_TEXTURE0_ARB );
	GL_TexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	GL_ClientActiveTextureARB (GL_TEXTURE0_ARB);

	if (glUnlockArraysEXT)
		glUnlockArraysEXT();

	arrays.numverts=arrays.numelems=0;

}

// assumes  2 Passes 
// we could extend this 
// TODO !!!
static void Render_Backend_Flush_Multitexture_Combine (shader_t *s,int lmtex )
{
	shaderpass_t * pass;
	int texture  ;

	if (s->numpasses != 2 )
		return ;

	Render_Backend_Make_Vertices (s);


	
	switch (s->cull )
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
		glLockArraysEXT(0,arrays.numverts);

	












}


// TODO !!!
static void Render_Backend_Flush_Vertex_Lit (shader_t *s, int lmtex )
{




}
