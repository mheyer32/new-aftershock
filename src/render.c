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
#include "util.h"
#include "a_shared.h"
#include "cmap.h"
#include "shader.h"
#include "mapent.h"
#include "render.h"
#include "renderback.h"
#include <math.h>
#include "opengl.h"
#include "md3.h"
#include "c_var.h"
#include "io.h"
#include "matrix.h"
#include "console.h"


/* The front end of the rendering pipeline decides what to draw, and
 * the back end actually draws it.  These functions build a list of faces
 * to draw, sorts it by shader, and sends it to the back end (renderback.c)
 */

/* cvars */
cvar_t *con_notifytime;
cvar_t *r_allowExtensions;
cvar_t *r_allowSoftwareGL;
cvar_t *r_clear;
cvar_t *r_colorbits;
cvar_t *r_colorMipLevels;
cvar_t *r_depthbits;
#ifdef _WIN32
cvar_t *r_displayrefresh;
#endif
cvar_t *r_drawBuffer;
cvar_t *r_drawentities;
cvar_t *r_drawworld;
cvar_t *r_dynamiclight;
cvar_t *r_ext_compiled_vertex_array;
cvar_t *r_ext_compress_textures;
#ifdef _WIN32
cvar_t *r_ext_gamma_control;
#endif
cvar_t *r_ext_multitexture;
#ifndef __linux__
cvar_t *r_ext_swap_control;
#endif
cvar_t *r_ext_texture_env_add;
cvar_t *r_facePlaneCull;
cvar_t *r_fastsky;
cvar_t *r_finish;
cvar_t *r_fullbright;
cvar_t *r_fullscreen;
cvar_t *r_gamma;
cvar_t *r_glDriver;
cvar_t *r_ignoreFastPath;
cvar_t *r_ignoreGLErrors;
cvar_t *r_lastValidRenderer;
cvar_t *r_lightmap;
cvar_t *r_lockpvs;
cvar_t *r_lodbias;
cvar_t *r_logFile;
cvar_t *r_mapOverBrightBits;
cvar_t *r_mode;
cvar_t *r_nocurves;
cvar_t *r_offsetfactor;
cvar_t *r_offsetunits;
cvar_t *r_overBrightBits;
cvar_t *r_picmip;
cvar_t *r_primitives;
cvar_t *r_printShaders;
cvar_t *r_roundImagesDown;
cvar_t *r_screenshot_format;
cvar_t *r_showcluster;
cvar_t *r_showImages;
cvar_t *r_shownormals;
cvar_t *r_showsky;
cvar_t *r_showtris;
cvar_t *r_speeds;
cvar_t *r_stencilbits;
cvar_t *r_stereo;
cvar_t *r_subdivisions;
cvar_t *r_swapInterval;
cvar_t *r_texturebits;
cvar_t *r_textureMode;
cvar_t *r_verbose;
cvar_t *r_vertexLight;
cvar_t *r_znear;
cvar_t *vid_xpos;
cvar_t *vid_ypos;
cvar_t *win_wndproc;


typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int	flags;
} cvarTable_t;

static cvarTable_t cvarTable[] = {
	{ &con_notifytime, "con_notifytime", "3", 0 },
	{ &r_allowExtensions, "r_allowExtensions", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_allowSoftwareGL, "r_allowSoftwareGL", "0", CVAR_ARCHIVE },
	{ &r_clear, "r_clear", "0", CVAR_ARCHIVE | CVAR_CHEAT },
	{ &r_colorbits, "r_colorbits", "0", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_colorMipLevels, "r_colorMipLevels", "0", CVAR_LATCH },
	{ &r_depthbits, "r_depthbits", "0", CVAR_ARCHIVE | CVAR_LATCH },
#ifdef _WIN32
	{ &r_displayrefresh, "r_displayrefresh", "0", CVAR_ARCHIVE | CVAR_LATCH },
#endif
	{ &r_drawBuffer, "r_drawBuffer", "GL_BACK", 0 },
	{ &r_drawentities, "r_drawentities", "1", CVAR_CHEAT },
	{ &r_drawworld, "r_drawworld", "1", CVAR_CHEAT },
	{ &r_dynamiclight, "r_dynamiclight", "1", CVAR_ARCHIVE },
	{ &r_ext_compiled_vertex_array, "r_ext_compiled_vertex_array", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_ext_compress_textures, "r_ext_compress_textures", "1", CVAR_ARCHIVE | CVAR_LATCH },
#ifdef _WIN32
	{ &r_ext_gamma_control, "r_ext_gamma_control", "1", CVAR_ARCHIVE | CVAR_LATCH },
#endif
	{ &r_ext_multitexture, "r_ext_multitexture", "1", CVAR_ARCHIVE | CVAR_LATCH },
#ifdef _WIN32
	{ &r_ext_swap_control, "r_ext_swap_control", "0", CVAR_ARCHIVE | CVAR_LATCH },
#endif
	{ &r_ext_texture_env_add, "r_ext_texture_env_add", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_facePlaneCull, "r_facePlaneCull", "1", CVAR_ARCHIVE },
	{ &r_fastsky, "r_fastsky", "0", CVAR_ARCHIVE },
	{ &r_finish, "r_finish", "0", CVAR_ARCHIVE },
	{ &r_fullbright, "r_fullbright", "0", CVAR_CHEAT | CVAR_LATCH },
	{ &r_fullscreen, "r_fullscreen", "0", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_gamma, "r_gamma", "1", CVAR_ARCHIVE },
	{ &r_glDriver, "r_glDriver", "opengl32", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_ignoreFastPath, "r_ignoreFastPath", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_ignoreGLErrors, "r_ignoreGLErrors", "1", CVAR_ARCHIVE },
	{ &r_lastValidRenderer, "r_lastValidRenderer", "(uninitialized)", CVAR_ARCHIVE  },
	{ &r_lightmap, "r_lightmap", "0", CVAR_CHEAT },
	{ &r_lockpvs, "r_lockpvs", "0", CVAR_CHEAT },
	{ &r_lodbias, "r_lodbias", "0", CVAR_ARCHIVE}, // NEW
	{ &r_logFile, "r_logFile", "0", CVAR_CHEAT },
	{ &r_mapOverBrightBits, "r_mapOverBrightBits", "2", CVAR_LATCH },
	{ &r_mode, "r_mode", "3", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_nocurves, "r_nocurves", "0", CVAR_CHEAT },
	{ &r_offsetfactor, "r_offsetfactor", "-1", CVAR_CHEAT },
	{ &r_offsetunits, "r_offsetunits", "-2", CVAR_CHEAT },
	{ &r_overBrightBits, "r_overBrightBits", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_picmip, "r_picmip", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_primitives, "r_primitives", "0", CVAR_ARCHIVE },
	{ &r_printShaders, "r_printShaders", "0", 0 },
	{ &r_roundImagesDown, "r_roundImagesDown", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_screenshot_format, "r_screenshot_format", "0", CVAR_ARCHIVE },
	{ &r_showcluster, "r_showcluster", "0", CVAR_CHEAT },
	{ &r_showImages, "r_showImages", "0", 0 },
	{ &r_shownormals, "r_shownormals", "0", CVAR_CHEAT },
	{ &r_showsky, "r_showsky", "0", CVAR_CHEAT },
	{ &r_showtris, "r_showtris", "0", CVAR_CHEAT },
	{ &r_speeds, "r_speeds", "0", CVAR_CHEAT },
	{ &r_stencilbits, "r_stencilbits", "8", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_stereo, "r_stereo", "0", CVAR_ARCHIVE },
	{ &r_subdivisions, "r_subdivisions", "4", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_swapInterval, "r_swapInterval", "1", CVAR_ARCHIVE },
	{ &r_texturebits, "r_texturebits", "0", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_textureMode, "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST", CVAR_ARCHIVE },
	{ &r_verbose, "r_verbose", "0", CVAR_CHEAT },
	{ &r_vertexLight, "r_vertexLight", "0", CVAR_ARCHIVE | CVAR_LATCH },
	{ &r_znear, "r_znear", "4", CVAR_CHEAT },
	{ &vid_xpos, "vid_xpos", "3", CVAR_ARCHIVE },
	{ &vid_ypos, "vid_ypos", "22", CVAR_ARCHIVE}
};

const static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);



void R_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}


void R_Draw_World (void );
void R_Setup_Clipplanes (const refdef_t * fd );
void R_Recursive_World_Node (int n);
void R_Render_Walk_Face (int num );
int R_ClipFrustrum (vec3_t mins ,vec3_t maxs );

void R_Render_Poly (refEntity_t *e );
void R_Render_Sprite (const refEntity_t * re );
void R_Render_Beam ( refEntity_t *e );
void R_Render_RailCore (refEntity_t *e);
void R_Render_RailRings (refEntity_t *e );
void R_Render_Bsp_Model (int num );
void R_Render_Lightning (refEntity_t *e );
void R_Render_PortalSurface (refEntity_t *e );

unsigned int SortKey (cface_t * face );

static int R_Find_Cluster(const vec3_t pos);
static void sort_faces(void);

static facelist_t facelist;   /* Faces to be drawn */
static facelist_t translist;  /* Transparent faces to be drawn */
static int r_leafcount;       /* Counts up leafs walked for this scene */
static byte *r_faceinc;        /* Flags faces as "included" in the facelist */
static int *skylist;          /* Sky faces hit by walk */
static int numsky;            /* Number of sky faces in list */
static float cos_fov;         /* Cosine of the field of view angle */

int * r_lightmaps = NULL;
int  r_numLightmaps = 0;


#define  MAX_REF_ENTITIES 256 
#define  MIN_RENDER_LIST_SIZE 512
 
#define  MAX_DYN_POLYS 512
#define	 MAX_VERTS_ON_POLY	10
#define  MAX_POLY_VERTS MAX_DYN_POLYS * MAX_VERTS_ON_POLY 



static refEntity_t refEntities[MAX_REF_ENTITIES];
static int numref_entities=0;

static refdef_t r_render_def ;

quad_t overlay [MAX_OVERLAY];
static int overlay_numquads =0;

rendface_t *render_list =NULL ;
static int r_num_render_list_elems=0;

// Dynamic Lightning :
dlight_t r_dlights [MAX_DLIGHTS];
int r_num_dlights =0;

static polyVert_t PolyVerts [MAX_POLY_VERTS];
static int num_PolyVerts=0;

static poly_t Dyn_Polys[MAX_DYN_POLYS];
static int dyn_polys_count=0;

static cplane_t clipplanes[4];

static mat4_t model_view_mat ;

int r_WorldMap_loaded = 0;

// This will be used by the backend when doing sfx like environment-mapping
reference_t transform_ref;

colour_t r_actcolor={255,255,255,255};



void R_Init(void)
{

	Con_Printf (" -------- R_INIT : --------- \n");

	R_GetCvars( );

	if (!Init_OpenGL ())
		Error ("Could not Init OpenGL ! ");

	if (!Shader_Init())
		Error ("Could not Init Shaders !");
	
	if (!MD3_Init ())
		Error ("Could not Init MD3 !");

	GL_DepthMask (GL_TRUE);

	glFinish();
    GL_EnableClientState(GL_VERTEX_ARRAY);
	GL_EnableClientState(GL_TEXTURE_COORD_ARRAY);
	GL_EnableClientState(GL_COLOR_ARRAY);

	GL_Disable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);


    GL_Enable(GL_DEPTH_TEST);
    GL_CullFace(GL_FRONT);
	GL_Disable(GL_CULL_FACE);
    GL_Enable(GL_TEXTURE_2D);
    GL_TexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glPolygonOffset (r_offsetfactor->value, r_offsetunits->value);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    R_backend_init();

	Con_Printf (" ... finished R_Init ... \n");

}

void R_Shutdown (void )
{

	glFinish ();

	Shader_Shutdown();
	MD3_Shutdown ();

	free(facelist.faces);
    free(translist.faces);
    free(r_faceinc);
    free(skylist);

	free (render_list);

	R_backend_shutdown();


	Shutdown_OpenGL();

}

void R_AddPolyToScene (const polyVert_t *verts, int numVerts, int Shader) 
{
	poly_t *p;

	if ((dyn_polys_count >= MAX_DYN_POLYS) || 
		!verts || 
		!numVerts)
		return;

	p = &Dyn_Polys[dyn_polys_count++];
	p->hShader = Shader;
	p->numVerts = numVerts;
	
	memcpy (&PolyVerts[num_PolyVerts], verts, numVerts * sizeof(polyVert_t));

	p->verts = &PolyVerts[num_PolyVerts];
	num_PolyVerts += numVerts;

	// TODO :
	// PUSH to the render_list
}

// TODO : Optimize : Sort !
void R_RenderPolys (void )
{
	int i, j;
	poly_t *p;

	for (i = 0; i < dyn_polys_count; i++)
	{
		p = &Dyn_Polys [i];

		for (j = 0; j < p->numVerts; j++)
		{
			VectorCopy(p->verts[j].xyz, arrays.verts[arrays.numverts]);
			arrays.tex_st[arrays.numverts][0] = p->verts[j].st[0];
			arrays.tex_st[arrays.numverts][1] = p->verts[j].st[1];
			colour_copy (p->verts[j].modulate, arrays.colour[j]);
			arrays.elems[arrays.numverts] = arrays.numverts+j; // FIXME ?
			arrays.numverts++;
		}

		Render_Backend_Flush (p->hShader,0);
	}
}

// TODO:Make resolution independent !
void R_DrawString( int x, int y, const char *str, vec4_t color )
{
	const char *s;
	char	ch;
	float	frow;
	float	fcol;
	vec4_t	tempcolor;

	// offscreen
	if (y < -SMALLCHAR_HEIGHT)
		return;

	// draw the colored text
	R_SetColor( color );
	
	s = str;

	while ( *s ) {
		if ( A_IsColorString( s ) ) {
			memcpy( tempcolor, a_color_table[ColorIndex(s[1])], sizeof( tempcolor ) );
			tempcolor[3] = color[3];
			R_SetColor( tempcolor );

			s += 2;
			continue;
		}

		ch = *s & 255;
		if (ch != ' ') {
			frow = (ch >> 4) * .0625f;
			fcol = (ch & 15) * .0625f;
			R_DrawStretchPic( (float)x, (float)y, (float)SMALLCHAR_WIDTH, (float)SMALLCHAR_HEIGHT, fcol, frow, fcol + .0625f, frow + .0625f, shader_text );
		}

		x += SMALLCHAR_WIDTH;
		s++;
	}
}

void R_DrawStretchPic (float x, float y, float w, float h, float s1, float t1, float s2, float t2, int hShader)
{
	vec3_t *vecs;
	vec2_t *tc;
	colour_t *color;
	quad_t *q;
	int *elems;

	if (hShader < 0) 
		return;

	q = &overlay[overlay_numquads++];
	vecs = q->verts;
	tc = q->tc;
	color = q->color;
	elems = q->elems;
	q->shader = hShader;

	vecs[0][0] = x;
	vecs[0][1] = y;
	vecs[0][2] = 0;
	tc[0][0] = s1;
	tc[0][1] = t1;
	colour_copy (r_actcolor, color[0]);

	vecs[1][0] = x + w;
	vecs[1][1] = y;
	vecs[1][2] = 0;
	tc[1][0] = s2;
	tc[1][1] = t1;
	colour_copy (r_actcolor, color[1]);

	vecs[2][0] = x + w;
	vecs[2][1] = y + h;
	vecs[2][2] = 0;
	tc[2][0] = s2;
	tc[2][1] = t2;
	colour_copy (r_actcolor, color[2]);
	
	vecs[3][0] = x;
	vecs[3][1] = y + h;
	vecs[3][2] = 0;
	tc[3][0] = s1;
	tc[3][1] = t2;
	colour_copy (r_actcolor, color[3]);

	elems[0] = 0;
	elems[1] = 1;
	elems[2] = 2;
	elems[3] = 0;
	elems[4] = 2;
	elems[5] = 3;
}

void R_SetColor (const float *rgba)
{
	if (!rgba)
	{ 
		r_actcolor[0] = 255;
		r_actcolor[1] = 255;
		r_actcolor[2] = 255;
		r_actcolor[3] = 255;
	
		return;
	}
 
	r_actcolor[0] = (byte)(rgba[0]*255.0);
	r_actcolor[1] = (byte)(rgba[1]*255.0);
	r_actcolor[2] = (byte)(rgba[2]*255.0);
	r_actcolor[3] = (byte)(rgba[3]*255.0);
}

void R_ClearScene (void)
{
	numref_entities = 0;
	overlay_numquads = 0;
	num_PolyVerts = 0;
	dyn_polys_count = 0;
	r_num_dlights = 0;
}

void R_renderEntities (void)
{
	int i;

	for (i = 0; i < numref_entities; i++)
	{
		switch (refEntities[i].reType)
		{
		case RT_MODEL:
			R_render_model (&refEntities[i]);
			break;
		case RT_POLY:
			R_Render_Poly(&refEntities[i]);
			break;
		case RT_SPRITE:
			R_Render_Sprite (&refEntities[i]);
			break;
		case RT_BEAM:
			R_Render_Beam (&refEntities[i]);
			break;
		case RT_RAIL_CORE:
			R_Render_RailCore (&refEntities[i]);
			break;
		case RT_RAIL_RINGS:
			R_Render_RailRings (&refEntities[i]);
			break;
		case RT_LIGHTNING:
			R_Render_Lightning (&refEntities[i]);
			break;
		case RT_PORTALSURFACE:
			R_Render_PortalSurface (&refEntities[i]);
			break;
	
		default :
			break;
		}
	}
}

void R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b )
{
	if (r_num_dlights >= MAX_DLIGHTS)
		return;

	VectorCopy(org, r_dlights[r_num_dlights].origin);

	r_dlights[r_num_dlights].r = r;
	r_dlights[r_num_dlights].g = g;
	r_dlights[r_num_dlights].b = b;

	r_dlights[r_num_dlights++].intensity = intensity;
}

void R_AddRefEntityToScene( const refEntity_t *re ) 
{
	memcpy(&refEntities[numref_entities],re,sizeof (refEntity_t ));
	numref_entities++;
}

void interpolate_normal( vec3_t  n1, vec3_t n2, float frac, vec3_t nI)
{
	int v;

	for (v = 0; v < 3; v++) 
	{
		nI[v] = (1.0f - frac) * n1[v] + frac * n2[v];
	}
}

void R_LerpTag(orientation_t *tag, int model, int startFrame, int endFrame, float frac, const char *tagName) 
{
	int i,tagnum = -1;
	md3model2_t *mod;
	md3tag_t *st, *et;

	if ((model <= 0) || (model >= r_md3Modelcount))
		return;

	mod = &r_md3models [model];

	if (!mod->numframes)
		return;

	// if out of bounds, wrap
	if (mod->numframes <= startFrame)
		startFrame = 0;

	if (mod->numframes <= endFrame)
		endFrame = 0;

	if (startFrame < 0)
		startFrame = mod->numframes - 1;

	if (endFrame < 0) 
		endFrame = mod->numframes - 1;

	for (i = 0; i< mod->numtags; i++)
	{
		if (!A_strncmp (tagName, mod->tags[startFrame][i].name, 12))
		{
			tagnum = i;
			break;
		}
	}

	if (tagnum < 0) 
		return;

	if (frac == 0.0)
	{
		VectorCopy(mod->tags[startFrame][tagnum].pos, tag->origin);
		VectorCopy(mod->tags[startFrame][tagnum].rot[0], tag->axis[0]);
		VectorCopy(mod->tags[startFrame][tagnum].rot[1], tag->axis[1]);
		VectorCopy(mod->tags[startFrame][tagnum].rot[2], tag->axis[2]);
	}
	else if (frac == 1.0)
	{
		VectorCopy(mod->tags[endFrame][tagnum].pos, tag->origin);
		VectorCopy(mod->tags[endFrame][tagnum].rot[0], tag->axis[0]);
		VectorCopy(mod->tags[endFrame][tagnum].rot[1], tag->axis[1]);
		VectorCopy(mod->tags[endFrame][tagnum].rot[2], tag->axis[2]);
	}
	else 
	{
		st = &mod->tags[startFrame][tagnum];
		et = &mod->tags[endFrame][tagnum];

		if (frac > 1.0 )
			frac = 1.0;

		if (frac < 0.0 )
			frac = 0.0;

		interpolate_normal(st->rot[0], et->rot[0], frac, tag->axis[0]);
		interpolate_normal(st->rot[1], et->rot[1], frac, tag->axis[1]);
		interpolate_normal(st->rot[2], et->rot[2], frac, tag->axis[2]);
		interpolate_normal(st->pos, et->pos, frac, tag->origin);
	}
}


// TODO: Optimize, put to backend 
void R_render_model (refEntity_t *re)
{
	mat4_t mat, loadmat, tmpmat;
	md3model2_t *model;
	md3mesh_t *mesh;
	skin_t *skin;
	uint_t *elem;
	int i, j, k, shaderref = -1;
	float saved_time;

	if ((re->hModel <= 0) || (re->hModel >= r_md3Modelcount))
		return;

	model = &r_md3models[re->hModel];
	arrays.numverts = arrays.numelems = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadMatrixf (model_view_mat);

	if (re->nonNormalizedAxes)
	{
		VectorNormalize (re->axis[0]);
		VectorNormalize (re->axis[1]);
		VectorNormalize (re->axis[2]);
	}

	Matrix4_Identity (tmpmat);
	tmpmat[12] = re->origin[0];
	tmpmat[13] = re->origin[1];
	tmpmat[14] = re->origin[2];

	Matrix4_Identity(mat);
	mat[0] = re->axis[0][0];
	mat[1] = re->axis[0][1];
	mat[2] = re->axis[0][2];

	mat[4] = re->axis[1][0];
	mat[5] = re->axis[1][1];
	mat[6] = re->axis[1][2];

	mat[8] = re->axis[2][0];
	mat[9] = re->axis[2][1];
	mat[10] = re->axis[2][2];
	
	Matrix4_Multiply(tmpmat, mat, loadmat);

	glMultMatrixf(loadmat);

	memcpy(transform_ref.matrix, loadmat, sizeof(mat4_t));
	transform_ref.inv_matrix_calculated = 0;
	transform_ref.matrix_identity = 0;

	// Set the shader time
	saved_time = g_frametime;

	// is this right?
	g_frametime = saved_time - (float)re->shaderTime / 1000.0f;

	for (j = 0; j < model->nummeshes; j++)
	{
		mesh = &model->meshes[j];
		shaderref = -1;

		if (re->customShader > 0)
		{
			shaderref = re->customShader;
		}
		else if (re->customSkin > 0)
		{
			skin = &md3skins[re->customSkin];

			for (i = 0; i < skin->num_mesh_skins; i++)
			{
				if (!stricmp(skin->skins[i].mesh_name, mesh->name))
				{
					shaderref = skin->skins[i].shaderref;
					break;
				}
			}
			
			if (shaderref < 0)
				shaderref = skin->skins[0].shaderref;
		}
		else {
			shaderref = mesh->skins[re->skinNum];
		}

		arrays.numverts = 0;

	    elem = mesh->elems;
	    for (k = 0; k < mesh->numelems; k++)
	    {
			arrays.elems[arrays.numelems++] = arrays.numverts + *elem++;
	    }

		if (!re->backlerp)
		{
			for (k = 0; k < mesh->numverts; k++)
			{
				VectorCopy(mesh->points[re->frame][k], arrays.verts[arrays.numverts]);
				vec2_copy(mesh->tex_st[k], arrays.tex_st[arrays.numverts]);

				// Push the entity colour (TEST)
				colour_copy (re->shaderRGBA, arrays.entity_colour [arrays.numverts]);
				memset (arrays.colour [arrays.numverts], 255, 4);
				arrays.norms[arrays.numverts][0] = mesh->norms[re->frame][k][0];
				arrays.norms[arrays.numverts][1] = mesh->norms[re->frame][k][1];
				arrays.norms[arrays.numverts][2] = 0.0;
				arrays.numverts++;
			}
		}
		else 
		{
			float frac = 1.0 - re->backlerp;
			int backframe = re->oldframe;
			
			if (backframe < 0)   // wrap
				backframe = mesh->numframes - 1;

			if (backframe > mesh->numframes)
				backframe = 0;

			for (k = 0; k < mesh->numverts; k++)
			{
				arrays.norms[arrays.numverts][0] = mesh->norms[re->frame][k][0];
				arrays.norms[arrays.numverts][1] = mesh->norms[re->frame][k][1];
				arrays.norms[arrays.numverts][2] = 0.0;
				
				memset (arrays.colour [arrays.numverts], 255, 4);

				// Push the entity colour (TEST)
				colour_copy (re->shaderRGBA, arrays.entity_colour[arrays.numverts]);
			
				interpolate_normal(mesh->points[backframe][k], mesh->points[re->frame][k], frac, arrays.verts[arrays.numverts]);
				vec2_copy(mesh->tex_st[k], arrays.tex_st[arrays.numverts]);
				arrays.numverts++;
			}
		}
	
		Render_Backend_Flush(shaderref, 0);
	}

	g_frametime = saved_time;

	// Revert
	Matrix4_Identity(transform_ref.matrix);
	transform_ref.matrix_identity = 1;
	transform_ref.inv_matrix_calculated = 0;

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// Works :
// TODO : rotation !
void R_Render_Sprite (const refEntity_t * e )
{

	vec3_t v[4];
	vec2_t tc [4];
	vec3_t org ;
	int elems [6];
	vec3_t up ,right ;
	float rad ;
	vec3_t tmp ;
	int i;

	VectorCopy (e->origin,org );
	rad =e->radius;

	
	VectorCopy (r_render_def.viewaxis[1],up);
	VectorCopy (r_render_def.viewaxis[2],right );

	VectorAdd (up , right , tmp );
	VectorNormalize (tmp );
	VectorScale (tmp,rad ,tmp );

	// 1 
	VectorAdd (org , tmp , v[0]);

	// 3
	VectorSubtract (org ,tmp ,v[2]);

	VectorNegate(right, right);	
	VectorAdd ( up,right,tmp );
	VectorNormalize (tmp);

	VectorScale (tmp,rad ,tmp );
	
	// 2 
	VectorAdd (org , tmp , v[1]);

	// 4 
	VectorScale (tmp ,-1.0 , tmp );
	VectorAdd (org , tmp, v[3]);


	// TexCoords:

	tc[0][0]=0.0;
	tc[0][1]=0.0;

	tc[1][0]=0.0;
	tc[1][1]=1.0;

	tc[2][0]=1.0;
	tc[2][1]=1.0;

	tc[3][0]=1.0;
	tc[3][1]=0.0;



	// elems :
	elems[0]=0;
	elems[1]=1;
	elems[2]=2;

	elems[3]=2;
	elems[4]=3;
	elems[5]=0;


	// Push :
  for (i=0;i<6;i++)
	{
		arrays.elems[arrays.numelems++] = arrays.numverts + elems[i];

	}

	for (i=0;i<4;i++)
	{
		VectorCopy (v[i],arrays.verts[arrays.numverts]);
		arrays.tex_st[arrays.numverts][0]=tc[i][0];
		arrays.tex_st[arrays.numverts][1]=tc[i][1];
		arrays.numverts++;

	}

	Render_Backend_Flush(e->customShader, 0);

}

// TODO !!!
void R_Render_Poly (refEntity_t *e )
{



}

// TODO !!!
void R_Render_Beam ( refEntity_t *e )
{





}

// TODO !!!
void R_Render_RailCore (refEntity_t *e)
{








}

// TODO !!!
void R_Render_RailRings (refEntity_t *e )
{





}

// TODO !!!
void R_Render_Lightning (refEntity_t *e)
{


}

// TODO !!!
void R_Render_PortalSurface (refEntity_t *e )
{



}

void R_RenderScene( const refdef_t *fd ) 
{
	


	mat4_t modmat;

	memcpy (&r_render_def,fd,sizeof (refdef_t));


// Setup projection :
// WORKS !!!
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	GL_Perspective(fd->fov_y,fd->fov_x/fd->fov_y,r_znear->value,3000.0);

//Setup the Matrix :
// Finally WORKS !!!

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();

    glRotatef (-90,  1, 0, 0);	    // put Z going up
    glRotatef (90,  0, 0, 1);	    // put Z going up
   // glRotatef (-r_refdef.viewangles[2],  1, 0, 0);
   // glRotatef (-r_refdef.viewangles[0],  0, 1, 0);
   // glRotatef (-r_refdef.viewangles[1],  0, 0, 1);

	Matrix4_Identity(modmat);

	modmat[0]=fd->viewaxis[0][0];
	modmat[1]=fd->viewaxis[0][1];
	modmat[2]=fd->viewaxis[0][2];

	modmat[4]=fd->viewaxis[1][0];
	modmat[5]=fd->viewaxis[1][1];
	modmat[6]=fd->viewaxis[1][2];

	modmat [8]=fd->viewaxis[2][0];
	modmat [9]=fd->viewaxis[2][1];
	modmat [10]=fd->viewaxis[2][2];


	glMultMatrixf(modmat);

    glTranslatef (-fd->vieworg[0],  -fd->vieworg[1],  -fd->vieworg[2]);

	glGetFloatv ( GL_MODELVIEW_MATRIX , model_view_mat );


	glViewport(fd->x,winY-fd->height+fd->y,fd->width,fd->height); // TODO ?



	VectorCopy (fd->vieworg,r_eyepos);
	
// Reset the reference :
	Matrix4_Identity (transform_ref.matrix);
	transform_ref.matrix_identity = atrue;
	transform_ref.inv_matrix_calculated = afalse ;
	


// Make Clipplanes :
	R_Setup_Clipplanes (fd);

// Render World :
	if (r_drawworld->integer)
		if (!(fd->rdflags & RDF_NOWORLDMODEL) )
			R_Draw_World();

// Render Entities :
	if (r_drawentities->integer)
	R_renderEntities ();

	R_RenderPolys();



// Prepare for rendering overlay :
	glViewport(0, 0, winX, winY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winX, winY, 0, -999.000000, 999.000000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



}


 // TODO !!!
void R_Update_Screen (void )
{








}


static void R_Upload_Lightmaps (void )
{
	int i ,texsize = 128 * 128 * 3 ;
	
	r_numLightmaps = cm.lightmapdata_size / texsize ;

	r_lightmaps = malloc (r_numLightmaps * sizeof (int ));

	glGenTextures (r_numLightmaps , r_lightmaps );


	for (i=0; i< r_numLightmaps ; i++ )
	{
		// TODO :Apply Gammma ? 

		GL_BindTexture (GL_TEXTURE_2D,r_lightmaps [i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB,
		     GL_UNSIGNED_BYTE, &cm.lightmapdata[i * texsize]);
	}


}

static void R_Free_Lightmaps (void )
{
	glDeleteTextures (r_numLightmaps , r_lightmaps );

	free (r_lightmaps );

	r_numLightmaps =0;


}


// Loads and setups the map for Rendering :
void R_LoadWorldMap( const char *mapname )
{
	if (r_WorldMap_loaded )
		R_FreeWorldMap ();


	if (!CM_LoadMap (mapname , 1 ))
	{
		Con_Printf ("WARNING : R_LoadWorldMap : CM_LoadMap falied !\n");
		return ;
		
	}

	R_Upload_Lightmaps ();


	// sizeup the face-arrays :
	facelist.faces = malloc (cm.num_faces * sizeof (rendface_t ));
	facelist.numfaces =0;
	memset (facelist.faces ,0, cm.num_faces * sizeof ( rendface_t ));


	r_WorldMap_loaded =1;

}


void R_FreeWorldMap (void )
{
	R_Free_Lightmaps ();
	
	free (facelist.faces);

	r_WorldMap_loaded =0;
}



void R_StartFrame (void )
{
	
	GL_DepthMask(GL_TRUE);

	// TODO !!!
	if (!stricmp (r_drawBuffer->string, "GL_BACK"))
		glDrawBuffer (GL_BACK);
	else
		glDrawBuffer (GL_FRONT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}

void R_EndFrame (void )
{


	
	Render_backend_Overlay ( overlay,overlay_numquads );
	overlay_numquads =0;


	if (r_ext_swap_control->modified)
	{
		// Vic: doesn't work on my GeForce2
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(r_ext_swap_control->integer);

		r_ext_swap_control->modified=0;
	}

	glFinish();

	awglSwapLayerBuffers(dc,WGL_SWAP_MAIN_PLANE );
		


}

// TODO :
void R_Draw_World (void )
{
	int i;

	if (!r_drawworld->integer)
		return ;


	r_eyecluster=R_Find_Cluster(r_eyepos);


	for (i=0;i<cm.num_models;i++ )
		R_Render_Bsp_Model (i);




}

#define M_PI_2 M_PI /2.0f
void R_Setup_Clipplanes (const refdef_t * fd )
{
	float half_pi_minus_half_fov_x = M_PI_2 - fd->fov_x * M_PI / 360.f;
	float half_pi_minus_half_fov_y = M_PI_2 - fd->fov_y * M_PI / 360.f;

	// TODO !!!
	/* rotate VPN right by FOV_X/2 degrees */
	RotatePointAroundVector( clipplanes[0].normal, fd->viewaxis[2], fd->viewaxis[1], -half_pi_minus_half_fov_x);
	/* rotate VPN left by FOV_X/2 degrees */
	RotatePointAroundVector( clipplanes[1].normal, fd->viewaxis[2], fd->viewaxis[1], half_pi_minus_half_fov_x);
	/* rotate VPN up by FOV_Y/2 degrees */
	RotatePointAroundVector( clipplanes[2].normal, fd->viewaxis[0], fd->viewaxis[1], half_pi_minus_half_fov_y);
	/* rotate VPN down by FOV_Y/2 degrees */
	RotatePointAroundVector( clipplanes[3].normal, fd->viewaxis[0], fd->viewaxis[1], -half_pi_minus_half_fov_y);


	clipplanes[0].type = 3;
	clipplanes[0].dist = DotProduct (fd->vieworg,clipplanes[0].normal);
	SetPlaneSignbits (&clipplanes[0]);

	clipplanes[1].type = 3;
	clipplanes[1].dist = DotProduct (fd->vieworg,clipplanes[1].normal);
	SetPlaneSignbits (&clipplanes[1]);

	clipplanes[2].type = 3;
	clipplanes[2].dist = DotProduct (fd->vieworg,clipplanes[2].normal);
	SetPlaneSignbits (&clipplanes[2]);

	clipplanes[3].type = 3;
	clipplanes[3].dist = DotProduct (fd->vieworg,clipplanes[3].normal);
	SetPlaneSignbits (&clipplanes[3]);

}

void R_Render_Bsp_Model (int num )
{

	if (!num )
		R_Recursive_World_Node (0);
	else 
	{
		// TODO !!!
		
	}

}

// NO More accept , this slowed just down 
void R_Recursive_World_Node (int n)
{
	cnode_t *node ;
	cplane_t * plane ;
	float dist ;

	if (n<0 )
	{
		cleaf_t *leaf=&cm.leaves[-(n+1)];
		int i;

		if (r_eyecluster >= 0)
		if (! BSP_TESTVIS(r_eyecluster, leaf->cluster)) return;
		
		if (R_ClipFrustrum (leaf->mins,leaf->maxs))
			return ;

		i=leaf->firstface;

		while (i++<leaf->numfaces)
			R_Render_Walk_Face (cm.lfaces[i]);

	}
	else
	{
		node=&cm.nodes[n];

		if (R_ClipFrustrum (node->mins,node->maxs))
			return ;
		

		plane=node->plane;
		
		if (plane->type<3)
			dist = r_eyepos[plane->type] - plane->dist;
		else
			dist = DotProduct(r_eyepos,plane->normal) - plane->dist;
	
		
		if (dist> 0)
		{
			// IN FRONT :
			R_Recursive_World_Node (node->children[0]);

			R_Recursive_World_Node (node->children[1]);
		}
		else
		{
			// IN BACK :
			R_Recursive_World_Node (node->children[1]);

			R_Recursive_World_Node (node->children[0]);

		}


	}

}


int R_ClipFrustrum (vec3_t mins ,vec3_t maxs )
{
	static int res [4];

	res[0]=BoxOnPlaneSide(mins,maxs,&clipplanes[0]);
	if (res[0]==1) return 1;
	res[1]=BoxOnPlaneSide(mins,maxs,&clipplanes[1]);
	if (res[1]==1) return 1;
	res[2]=BoxOnPlaneSide(mins,maxs,&clipplanes[2]);
	if (res[2]==1) return 1;
	res[3]=BoxOnPlaneSide(mins,maxs,&clipplanes[3]);
	if (res[3]==1) return 1;

	return 0;
}

void R_Render_Walk_Face (int num )
{
	cface_t * face=&cm.faces[num];

	if (r_faceinc[num]) return;
    r_faceinc[num] = 1;


	switch (face->facetype)
	{
	case FACETYPE_NORMAL:
	case FACETYPE_TRISURF:
		// CULL the face : (this is not exact but it looks right )
		/*if (r_shaders[map.shadernums[face->shadernum]].flags & SHADER_DOCULL);
			if (face->v_norm[0]*(face->verts->v_point[0]-r_eyepos[0])
				+face->v_norm[1]*(face->verts->v_point[1]-r_eyepos[1])
				+face->v_norm[2]*(face->verts->v_point[2]-r_eyepos[2])>0)return;

		*/
		break;

	case FACETYPE_MESH :
		if (R_ClipFrustrum (face->mins,face->maxs))
			return ;


		break;

	default : // FLARE OR Error
		return ;

		break;
	
	}

	if (r_shaders[face->shadernum].flags & SHADER_SKY)
		skylist[numsky++] = num;
    else
	{
		// Use renderlist here 
		facelist.faces[facelist.numfaces].face = num;
		facelist.faces[facelist.numfaces++].sortkey = SortKey(face);
	}
}


int R_TestVis ( const vec3_t p1, const vec3_t p2 )
{
	int cluster1,cluster2;

	cluster1= R_Find_Cluster (p1);
	cluster2= R_Find_Cluster (p2);

	if ( cluster1 <0 || cluster2 <0 )
		return 1;

	return BSP_TESTVIS( cluster1 ,cluster2 );

}

// TODO !!!
unsigned int SortKey (cface_t * face )
{
/*
	return (r_shaders[map.shadernums[face->shadernum]].sort << 29 ) + // Needs 4 Bits 
		(r_shaders[map.shadernums[face->shadernum]].sortkey << 21) + // 8 Bits
		(face->shadernum << 7 ) + // 9 Bits 
		(face->lm_texnum ) ;  // 6 Bits
		*/
	return 0;
}

static int
R_Find_Cluster(const vec3_t pos)
{
	cnode_t *node;
    int cluster = -1;
    int leaf = -1;
	float dist;
	cplane_t *plane;
    
    node = cm.nodes;

    /* Find the leaf/cluster containing the given position */
    
    while (1)
    {
		plane=node->plane;
		if (plane->type<3)
			dist = pos[plane->type] - plane->dist;
		else 
			dist = vec_dot(pos,plane->normal) - plane->dist;
			

		
	if (dist > 0)
	{
	    if (node->children[0] < 0)
	    {
		leaf = -(node->children[0] + 1);
		break;
	    }
	    else
	    {
		node = &cm.nodes[node->children[0]];
	    }
	}
	else
	{
	    if (node->children[1] < 0)
	    {
		leaf = -(node->children[1] + 1);
		break;
	    }
	    else
	    {
		node = &cm.nodes[node->children[1]];
	    }
	}	    
    }

    if (leaf >= 0)
	{
		cluster =cm.leaves[leaf].cluster;
	}
    return cluster;
}


static int
face_cmp(const void *a, const void *b)
{
    return ((rendface_t*)a)->sortkey - ((rendface_t*)b)->sortkey;
}

static void
sort_faces(void)
{
    /* FIXME: expand qsort code here to avoid function calls */
    qsort((void*)facelist.faces, facelist.numfaces, sizeof(rendface_t),
	  face_cmp);
}

