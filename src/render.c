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
#include "mapent.h"
#include "render.h"
#include "renderback.h"
#include "opengl.h"
#include "md3.h"
#include "mesh.h"
#include "c_var.h"
#include "io.h"
#include "matrix.h"
#include "console.h"
#include "skybox.h"


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
cvar_t *r_ext_compressed_textures;
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
	{ &r_ext_compressed_textures, "r_ext_compressed_textures", "1", CVAR_ARCHIVE | CVAR_LATCH },
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

const static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

void R_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

void R_Draw_World (void);
static void R_Setup_Clipplanes (const refdef_t *fd);
static void R_Recursive_World_Node (int n);
static void R_Render_Walk_Face (int num);
static int  R_ClipFrustrum (vec3_t mins, vec3_t maxs);
static void R_RenderModel (const refEntity_t *re);
static void R_RenderPoly (const refEntity_t *re);
static void R_RenderSprite (const refEntity_t *re);
static void R_RenderBeam (const refEntity_t *re);
static void R_RenderRailCore (const refEntity_t *re);
static void R_RenderRailRings (const refEntity_t *re);
static void R_RenderBspModel (int num);
static void R_RenderLightning (const refEntity_t *re);
static void R_RenderPortalSurface (const refEntity_t *re);

static int R_Find_Cluster(const vec3_t pos);
static void sort_faces(void);

static facelist_t facelist;   // Faces to be drawn
static facelist_t translist;  // Transparent faces to be drawn
static int r_leafcount;       // Counts up leafs walked for this scene
static byte *r_faceinc;       // Flags faces as "included" in the facelist
static int *skylist;          // Sky faces hit by walk
static int numsky;            // Number of sky faces in list

unsigned int SortKey (cface_t *face);
uint_t *r_lightmaps = NULL;
int  r_numLightmaps = 0;

#define  MAX_REF_ENTITIES		256 
#define  MAX_DYN_POLYS			512
#define	 MAX_VERTS_ON_POLY		10
#define  MAX_POLY_VERTS			(MAX_DYN_POLYS * MAX_VERTS_ON_POLY)

static refEntity_t refEntities[MAX_REF_ENTITIES];
static int numRefEntities = 0;

static refdef_t r_render_def;
static refdef_t r_world_def;

// Dynamic Lightning
dlight_t dlights[MAX_DLIGHTS];
int numDlights = 0;

static polyVert_t PolyVerts[MAX_POLY_VERTS];
static int numPolyVerts = 0;

static poly_t DPolys[MAX_DYN_POLYS];
static int numDpolysCount = 0;

static cplane_t clipplanes[4];

aboolean r_WorldMap_loaded = afalse;

// This will be used by the backend when doing sfx like environment-mapping
reference_t transform_ref;

extern aboolean r_overlay;

void R_ClearScene (void);


void R_Init(void)
{
	Con_Printf ("-------- R_Init ---------\n");

	R_GetCvars( );

	if (!Init_OpenGL()) {
		Com_Error ( ERR_FATAL, "Could not Init OpenGL!");
		return;
	}

	if (!Shader_Init()) {
		Com_Error ( ERR_FATAL, "Could not Init Shaders!");
		return;
	}

	MD3_Init();

	GL_DepthMask (GL_TRUE);

	glFinish();
    GL_EnableClientState(GL_VERTEX_ARRAY);
	GL_EnableClientState(GL_TEXTURE_COORD_ARRAY);
	GL_EnableClientState(GL_COLOR_ARRAY);
	GL_EnableClientState(GL_NORMAL_ARRAY);

	GL_Disable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);

	r_overlay = afalse;

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

    R_BackendInit();

	R_ClearScene();

	Con_Printf ("... finished R_Init ...\n");
}

void R_Shutdown (void)
{
	glFinish ();

	Shader_Shutdown();

	MD3_Shutdown ();

	R_FreeWorldMap();

	R_BackendShutdown();

	Shutdown_OpenGL();
}

/*
==================
R_AddPolyToScene

==================
*/
void R_AddPolyToScene (const polyVert_t *verts, int numVerts, int Shader) 
{
	poly_t *p;

	if ((numDpolysCount >= MAX_DYN_POLYS) ||
		(Shader < 0) || 
		!verts || 
		!numVerts)
		return;

	p = &DPolys[numDpolysCount++];
	p->hShader = Shader;
	p->numVerts = numVerts;
	
	memcpy (&PolyVerts[numPolyVerts], verts, numVerts * sizeof(polyVert_t));

	p->verts = &PolyVerts[numPolyVerts];
	numPolyVerts += numVerts;
}

/*
=================
R_RenderPolys

TODO: Optimize and sort
=================
*/
static void R_RenderPolys( void )
{
	int i;
	poly_t *p;

	for (i = 0, p = &DPolys[0]; i < numDpolysCount; i++, p++)
	{
		R_BackendPushPoly (p);
		R_BackendFlush (p->hShader, 0);
	}
}

/*
==================
R_DrawString

TODO: Make resolution independent
==================
*/
void R_DrawString(int x, int y, const char *str, vec4_t color)
{
	const	char *s;
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

void R_ClearScene (void)
{
	numRefEntities = 0;
	numPolyVerts = 0;
	numDpolysCount = 0;
	numDlights = 0;
}

static void R_RenderEntities (void)
{
	int i;
	refEntity_t *refEnt = &refEntities[0];

	for (i = 0; i < numRefEntities; i++, refEnt++)
	{
		switch (refEnt->reType)
		{
			case RT_MODEL:
				R_RenderModel (refEnt);
				break;

			case RT_POLY:
				R_RenderPoly (refEnt);
				break;

			case RT_SPRITE:
				R_RenderSprite (refEnt);
				break;

			case RT_BEAM:
				R_RenderBeam (refEnt);
				break;

			case RT_RAIL_CORE:
				R_RenderRailCore (refEnt);
				break;

			case RT_RAIL_RINGS:
				R_RenderRailRings (refEnt);
				break;

			case RT_LIGHTNING:
				R_RenderLightning (refEnt);
				break;

			case RT_PORTALSURFACE:
				R_RenderPortalSurface (refEnt);
				break;
		
			default:
				break;
		}
	}
}

void R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b )
{
	if( numDlights >= MAX_DLIGHTS )
		return;

	VectorCopy( org, dlights[numDlights].origin );

	dlights[numDlights].r = r;
	dlights[numDlights].g = g;
	dlights[numDlights].b = b;
	dlights[numDlights++].intensity = intensity;
}

void R_AddRefEntityToScene( const refEntity_t *re ) 
{
	memcpy ( &refEntities[numRefEntities], re, sizeof (refEntity_t) );
	numRefEntities++;
}

void R_InterpolateNormal( vec3_t n1, vec3_t n2, float frac, vec3_t nI )
{
	if( frac < 0.01 ) {
		frac = 0.0f;
	} else if( frac > 0.99f ) {
		frac = 1.0f;
	}

	if( !frac ) {
		VectorCopy(n1, nI);
	} else if( frac == 1.0f ) {
		VectorCopy(n2, nI);
	} else {
		nI[0] = n1[0] + frac * (n2[0] - n1[0]);
		nI[1] = n1[1] + frac * (n2[1] - n1[1]);
		nI[2] = n1[2] + frac * (n2[2] - n1[2]);
	}
}

void R_LerpTag(orientation_t *tag, int modnum, int startFrame, int endFrame, float frac, const char *tagName) 
{
	int i, tagnum = -1;
	md3model2_t *model;

	if ((modnum < 1) || (modnum > r_md3Modelcount))
		return;

	model = &r_md3models[modnum-1];

	if (!model->numframes)
		return;

	// if out of bounds, wrap
	if (model->numframes <= startFrame)
		startFrame = 0;

	if (model->numframes <= endFrame)
		endFrame = 0;

	if (startFrame < 0)
		startFrame = model->numframes - 1;

	if (endFrame < 0) 
		endFrame = model->numframes - 1;

	for (i = 0; i < model->numtags; i++)
	{
		if (!A_strncmp (tagName, model->tags[startFrame][i].name, MAX_APATH))
		{
			tagnum = i;
			break;
		}
	}

	if (tagnum < 0) 
		return;

	R_InterpolateNormal(
		model->tags[startFrame][tagnum].orient.axis[0],
		model->tags[endFrame][tagnum].orient.axis[0],
		frac, tag->axis[0]);
	R_InterpolateNormal(
		model->tags[startFrame][tagnum].orient.axis[1], 
		model->tags[endFrame][tagnum].orient.axis[1], 
		frac, tag->axis[1]);
	R_InterpolateNormal(
		model->tags[startFrame][tagnum].orient.axis[2], 
		model->tags[endFrame][tagnum].orient.axis[2], 
		frac, tag->axis[2]);
	R_InterpolateNormal(
		model->tags[startFrame][tagnum].orient.origin, 
		model->tags[endFrame][tagnum].orient.origin, 
	  frac, tag->origin);
}

/*
=================
R_FindModelShader

Find the best suitable skin for the models mesh
=================
*/
ahandle_t R_FindMeshShader( const refEntity_t *re, md3mesh_t *mesh )
{
	if ( re->customShader > 0 ) {
		return re->customShader;
	} else if ( re->customSkin > 0 ) {
		int i;
		skin_t *skin = &md3skins[re->customSkin-1];
		
		for( i = 0; i < skin->num_mesh_skins; i++ )
			if( !A_stricmp( skin->skins[i].mesh_name, mesh->name ) )
				return skin->skins[i].shaderref;
	
		return skin->skins[0].shaderref;
	}

	return mesh->skins[re->skinNum];
}

/*
=================
R_FinishModel

Reset the matrix and pop an old one
=================
*/
void R_FinishModel( void )
{
	R_BackendClear();

	Matrix4_Identity(transform_ref.matrix);
	transform_ref.matrix_identity = atrue;
	transform_ref.inv_matrix_calculated = afalse;
		
	glPopMatrix();
}

void R_BackendInterpolateNormals( vec3_t *n1, vec3_t *n2, float frac, int numverts );

/*
=================
R_Render_Model

TODO: Optimize, put to backend.
=================
*/
static void R_RenderModel( const refEntity_t *re )
{
	md3model2_t *model;
	md3mesh_t *mesh;
	uint_t *elems;
	int j;
	ahandle_t shaderref = -1;
	int frame, backframe;
	float frac = 1.0f - re->backlerp;

	if( re->hModel < 1 || re->hModel > r_md3Modelcount )
		return;

	model = &r_md3models[re->hModel-1];

	if( !model || model->nummeshes < 0 )
		 return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	transform_ref.obj_matrix[0] = re->axis[0][0];
	transform_ref.obj_matrix[1] = re->axis[0][1];
	transform_ref.obj_matrix[2] = re->axis[0][2];
	transform_ref.obj_matrix[3] = 0;
	transform_ref.obj_matrix[4] = re->axis[1][0];
	transform_ref.obj_matrix[5] = re->axis[1][1];
	transform_ref.obj_matrix[6] = re->axis[1][2];
	transform_ref.obj_matrix[7] = 0;
	transform_ref.obj_matrix[8] = re->axis[2][0];
	transform_ref.obj_matrix[9] = re->axis[2][1];
	transform_ref.obj_matrix[10] = re->axis[2][2];
	transform_ref.obj_matrix[11] = 0;
	transform_ref.obj_matrix[12] = re->origin[0];
	transform_ref.obj_matrix[13] = re->origin[1];
	transform_ref.obj_matrix[14] = re->origin[2];
	transform_ref.obj_matrix[15] = 1.0;

	if ( re->nonNormalizedAxes ) {
		VectorNormalize( transform_ref.obj_matrix   );
		VectorNormalize(&transform_ref.obj_matrix[4]);
		VectorNormalize(&transform_ref.obj_matrix[8]);
	}

	Matrix4_Multiply (transform_ref.world_matrix, transform_ref.obj_matrix, transform_ref.matrix);

	glLoadMatrixf(transform_ref.matrix);

	transform_ref.inv_matrix_calculated = afalse;
	transform_ref.matrix_identity = afalse;

	// Set the shader time
	shadertime = cl_frametime - re->shaderTime;

	for( j = 0, mesh = &model->meshes[0]; j < model->nummeshes; j++, mesh++ ) {
		shaderref = R_FindMeshShader( re, mesh );
		
		if ( shaderref < 0 ) {
			R_FinishModel();
			return;
		}

		R_BackendClear();

		frame = re->frame;
		backframe = re->oldframe;
		elems = mesh->elems;

		if( re->renderfx & RF_WRAP_FRAMES ) {
			frame = frame % mesh->numframes;
			backframe = backframe % mesh->numframes;
		}

		if( frame < 0 || frame >= mesh->numframes || backframe < 0 || backframe >= mesh->numframes ) {
			frame = backframe = 0;
			frac = 0.0f;
		}

		R_BackendPushRaw( mesh->points[frame], mesh->norms[frame], mesh->tex_st, elems, mesh->numverts, mesh->numelems );
		R_BackendClearColor();
		R_BackendSetEntityColor( re->shaderRGBA );
		R_BackendInterpolateNormals ( mesh->points[backframe], mesh->points[frame], frac, mesh->numverts );
		R_BackendFlush( shaderref, 0 );
	}

	R_FinishModel();
}

/*
=================
R_Render_Sprite

TODO: rotation.
=================
*/
static void R_RenderSprite( const refEntity_t *re )
{
	quad_t q;
	vec3_t org, up, right, tmp;

	VectorCopy( re->origin, org );

	VectorCopy( r_render_def.viewaxis[1], up );
	VectorCopy( r_render_def.viewaxis[2], right );

	VectorAdd( up, right, tmp );
	VectorNormalize( tmp );
	VectorScale( tmp, re->radius, tmp );
	VectorAdd( org, tmp, q.verts[0] );
	VectorSubtract( org, tmp, q.verts[2] );
	VectorNegate( right, right );	
	VectorAdd( up, right, tmp );
	VectorNormalize( tmp );
	VectorScale( tmp, re->radius, tmp );
	VectorAdd( org, tmp, q.verts[1] );
	VectorScale( tmp, -1.0f, tmp );
	VectorAdd( org, tmp, q.verts[3] );

	// texcoords
	q.tc[0][0] = 0.0f;
	q.tc[0][1] = 0.0f;
	Vector4Copy( re->shaderRGBA, q.color[0] );

	q.tc[1][0] = 0.0f;
	q.tc[1][1] = 1.0f;
	Vector4Copy( re->shaderRGBA, q.color[1] );

	q.tc[2][0] = 1.0f;
	q.tc[2][1] = 1.0f;
	Vector4Copy( re->shaderRGBA, q.color[2] );

	q.tc[3][0] = 1.0f;
	q.tc[3][1] = 0.0f;
	Vector4Copy( re->shaderRGBA, q.color[3] );

	q.elems[0] = 0;
	q.elems[1] = 1;
	q.elems[2] = 2;
	q.elems[3] = 2;
	q.elems[4] = 3;
	q.elems[5] = 0;

	R_BackendPushQuad( &q );
	R_BackendFlush( re->customShader, 0 );
}

// TODO !!!
static void R_RenderPoly( const refEntity_t *re )
{
}

// TODO !!!
static void R_RenderBeam( const refEntity_t *re )
{
}

// TODO !!!
static void R_RenderRailCore( const refEntity_t *re )
{
}

// TODO !!!
static void R_RenderRailRings( const refEntity_t *re )
{
}

// TODO !!!
static void R_RenderLightning( const refEntity_t *re )
{
}

// TODO !!!
static void R_RenderPortalSurface( const refEntity_t *re )
{
}

void Matrix4_Rotate(mat4_t a, float angle, float x, float y, float z, mat4_t ret);

void R_RenderScene (const refdef_t *fd)
{
	memcpy (&r_render_def, fd, sizeof (refdef_t));

	// Setup projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
	GL_Perspective(fd->fov_y, fd->fov_x / fd->fov_y, r_znear->value, 3000.0);

	// Setup the Matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	transform_ref.world_matrix[0] = fd->viewaxis[1][0];
	transform_ref.world_matrix[1] = 0;
	transform_ref.world_matrix[2] = -1;
	transform_ref.world_matrix[3] = 0;
	transform_ref.world_matrix[4] = -fd->viewaxis[0][0];
	transform_ref.world_matrix[5] = -fd->viewaxis[0][1];
	transform_ref.world_matrix[6] = -fd->viewaxis[0][2];
	transform_ref.world_matrix[7] = 0;
	transform_ref.world_matrix[8] = fd->viewaxis[2][0];
	transform_ref.world_matrix[9] = 1;
	transform_ref.world_matrix[10] = 0;
	transform_ref.world_matrix[11] = 0;
	transform_ref.world_matrix[12] = fd->vieworg[0];
	transform_ref.world_matrix[13] = fd->vieworg[1];
	transform_ref.world_matrix[14] = fd->vieworg[2];
	transform_ref.world_matrix[15] = 1;

	// Load a rotated matrix
	glLoadMatrixf (transform_ref.world_matrix);
	glViewport(fd->x, winY - fd->height - fd->y, fd->width, fd->height); // TODO ?

	VectorCopy (fd->vieworg, r_eyepos);
	
	// Reset the reference
	Matrix4_Identity (transform_ref.matrix);
	transform_ref.matrix_identity = atrue;
	transform_ref.inv_matrix_calculated = afalse;

	// Make Clipplanes
	R_Setup_Clipplanes(fd);

	// Render World
	if (r_drawworld->integer)
		if (!(fd->rdflags & RDF_NOWORLDMODEL))
			R_Draw_World();

	// Render Entities
	if (r_drawentities->integer)
		R_RenderEntities();

	R_RenderPolys();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// TODO !!!
void R_Update_Screen (void)
{
}


static void R_Upload_Lightmaps (void)
{
	int i, texsize = LIGHTMAP_WIDTH * LIGHTMAP_HEIGHT * 3;
	
	r_numLightmaps = cm.lightmapdata_size / texsize;

	r_lightmaps = malloc (r_numLightmaps * sizeof (uint_t));

	glGenTextures (r_numLightmaps, r_lightmaps);

	for (i = 0; i < r_numLightmaps; i++)
	{
		// TODO: Apply gammma? 
		GL_BindTexture (GL_TEXTURE_2D, r_lightmaps[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT, 0, GL_RGB,
		     GL_UNSIGNED_BYTE, &cm.lightmapdata[i * texsize]);
	}
}

static void R_Free_Lightmaps (void)
{
	glDeleteTextures (r_numLightmaps, r_lightmaps);

	free (r_lightmaps);

	r_numLightmaps = 0;
}

// Loads and setups the map for Rendering
void R_LoadWorldMap (const char *mapname)
{
	if (r_WorldMap_loaded)
		R_FreeWorldMap();

	if (!cm.name[0]) {
		if (!CM_LoadMap (mapname, atrue))
		{
			Con_Printf (S_COLOR_YELLOW "WARNING: R_LoadWorldMap: CM_LoadMap failed!\n");
			return;
		}
	}

	R_Upload_Lightmaps ();

	// sizeup the face-arrays
	facelist.faces = malloc (cm.num_faces * sizeof (rendface_t));
	facelist.numfaces = 0;
	memset (facelist.faces, 0, cm.num_faces * sizeof (rendface_t));

	r_faceinc = malloc (cm.num_faces);
	memset (r_faceinc, 0, cm.num_faces);

    skylist = (int *)malloc(100 * sizeof(int));
	memset (skylist, 0, 100 * sizeof (int));

	r_WorldMap_loaded = atrue;

	Mesh_CreateAll();
    SkyboxCreate();
}

void R_FreeWorldMap (void)
{
	if (!r_WorldMap_loaded)
		return;

	R_Free_Lightmaps ();
	
	SkyboxFree();
	Mesh_FreeAll();

	free(facelist.faces);
    free(translist.faces);
    free(r_faceinc);
	free(skylist);

	r_WorldMap_loaded = afalse;
}

void R_StartFrame (void)
{
	GL_DepthMask(GL_TRUE);

	// TODO !!!
	if (!A_stricmp (r_drawBuffer->string, "GL_BACK"))
		glDrawBuffer (GL_BACK);
	else
		glDrawBuffer (GL_FRONT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DoGamma(void);

void R_EndFrame (void)
{
	if (r_gamma->value != 1.0f)
		DoGamma();

	if (r_ext_swap_control->modified)
	{
		// Vic: doesn't work on my GeForce2
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(r_ext_swap_control->integer);

		r_ext_swap_control->modified = 0;
	}

	glFinish();

	if (awglSwapLayerBuffers)
		awglSwapLayerBuffers(dc, WGL_SWAP_MAIN_PLANE);
	else
		awglSwapBuffers(dc);
}

// TODO
void R_Draw_World (void)
{
	int i;

	if (!r_drawworld->integer)
		return;

    facelist.numfaces = 0;
    numsky = 0;

    // Clear "included" faces lists
    memset(r_faceinc, 0, cm.num_faces * sizeof(byte));

	VectorSet (r_eyepos, 0, 0, 0);

	r_eyecluster = R_Find_Cluster (r_eyepos);

	for (i = 0; i < cm.num_models; i++)
		R_RenderBspModel (i);

    // Sort the face list
	sort_faces();

    // Draw sky first
	if (numsky) {
		R_BackendSky(numsky, skylist);
	}

    // Draw normal faces
	Render_Backend(&facelist);
}

#define M_PI_2 M_PI*0.5f

static void R_Setup_Clipplanes (const refdef_t *fd)
{
	float half_pi_minus_half_fov_x = M_PI_2 - fd->fov_x * M_PI / 360.f;
	float half_pi_minus_half_fov_y = M_PI_2 - fd->fov_y * M_PI / 360.f;

	// TODO !!!
	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector( clipplanes[0].normal, fd->viewaxis[2], fd->viewaxis[1], -half_pi_minus_half_fov_x);

	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector( clipplanes[1].normal, fd->viewaxis[2], fd->viewaxis[1], half_pi_minus_half_fov_x);

	// rotate VPN up by FOV_Y/2 degrees
	RotatePointAroundVector( clipplanes[2].normal, fd->viewaxis[0], fd->viewaxis[1], half_pi_minus_half_fov_y);

	// rotate VPN down by FOV_Y/2 degrees
	RotatePointAroundVector( clipplanes[3].normal, fd->viewaxis[0], fd->viewaxis[1], -half_pi_minus_half_fov_y);

	clipplanes[0].type = 3;
	clipplanes[0].dist = DotProduct (fd->vieworg, clipplanes[0].normal);
	SetPlaneSignbits (&clipplanes[0]);

	clipplanes[1].type = 3;
	clipplanes[1].dist = DotProduct (fd->vieworg, clipplanes[1].normal);
	SetPlaneSignbits (&clipplanes[1]);

	clipplanes[2].type = 3;
	clipplanes[2].dist = DotProduct (fd->vieworg, clipplanes[2].normal);
	SetPlaneSignbits (&clipplanes[2]);

	clipplanes[3].type = 3;
	clipplanes[3].dist = DotProduct (fd->vieworg, clipplanes[3].normal);
	SetPlaneSignbits (&clipplanes[3]);
}

void R_RenderBspModel (int num)
{
	if (!num)
		R_Recursive_World_Node (0);
	else 
	{
		// TODO !!!
	}
}

// NO More accept, this slowed just down 
static void R_Recursive_World_Node (int n)
{
	cnode_t *node;
	cplane_t *plane;
	float dist;

	if (n < 0)
	{
		cleaf_t *leaf = &cm.leaves[~n];
		int i;

//		if (r_eyecluster >= 0)
//			if (!BSP_TESTVIS(r_eyecluster, leaf->cluster)) 
//				return;
		
//		if (R_ClipFrustrum (leaf->mins, leaf->maxs))
//			return;

		for (i = 0; i < leaf->numfaces; i++)
			R_Render_Walk_Face (cm.lfaces[leaf->firstface + i]);
	}
	else
	{
		node = &cm.nodes[n];

//		if (R_ClipFrustrum (node->mins, node->maxs))
//			return;
		
		plane = node->plane;
		
		if (plane->type < 3)
			dist = r_eyepos[plane->type] - plane->dist;
		else
			dist = DotProduct(r_eyepos, plane->normal) - plane->dist;
	
		R_Recursive_World_Node (node->children[(dist <= 0)]);
		R_Recursive_World_Node (node->children[(dist > 0)]);
	}
}


static int R_ClipFrustrum (vec3_t mins, vec3_t maxs)
{
	if (BoxOnPlaneSide(mins, maxs, &clipplanes[0]) == 1)
		return 1;
	if (BoxOnPlaneSide(mins, maxs, &clipplanes[1]) == 1)
		return 1;
	if (BoxOnPlaneSide(mins, maxs, &clipplanes[2]) == 1)
		return 1;
	if (BoxOnPlaneSide(mins, maxs, &clipplanes[3]) == 1)
		return 1;

	return 0;
}

static void R_Render_Walk_Face (int num)
{
	cface_t *face = &cm.faces[num];

	if (r_faceinc[num]) 
		return;

    r_faceinc[num] = 1;

	switch (face->facetype)
	{
		case FACETYPE_NORMAL:
		case FACETYPE_TRISURF:
			// CULL the face : (this is not exact but it looks right )
			if (r_shaders[cm.shaderrefs[face->shadernum].shadernum].flags & SHADER_DOCULL)
				if (face->verts->v_norm[0]*(face->verts->v_point[0]-r_eyepos[0])
					+face->verts->v_norm[1]*(face->verts->v_point[1]-r_eyepos[1])
					+face->verts->v_norm[2]*(face->verts->v_point[2]-r_eyepos[2]) > 0)
					return;
		break;

		case FACETYPE_MESH:
//			if (R_ClipFrustrum (face->mins, face->maxs))
//				return;
			break;

		default: // FLARE OR Error
			return;
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

int R_TestVis (const vec3_t p1, const vec3_t p2)
{
	int cluster1 = R_Find_Cluster (p1), 
		cluster2 = R_Find_Cluster (p2);

	if ((cluster1 < 0) || (cluster2 < 0))
		return 1;

	return BSP_TESTVIS (cluster1, cluster2);
}

// TODO !!!
unsigned int SortKey (cface_t *face)
{
/*
	return (r_shaders[cs.shadernums[face->shadernum]].sort << 29 ) + // Needs 4 Bits 
		(r_shaders[map.shadernums[face->shadernum]].sortkey << 21) + // 8 Bits
		(face->shadernum << 7 ) + // 9 Bits 
		(face->lm_texnum ) ;  // 6 Bits
*/		
/*
	return (r_shaders[face->shadernum].sort << 29) + // Needs 4 Bits 
		(r_shaders[face->shadernum].sortkey << 21) + // 8 Bits
		(face->shadernum << 7) + // 9 Bits 
		(face->lightmapnum);  // 6 Bits
*/
	return 0;
}

static int R_Find_Cluster(const vec3_t pos)
{
    int cluster = -1;
    int leaf = -1;
	float dist;
	cplane_t *plane;
 	cnode_t *node = cm.nodes;
   
    // Find the leaf/cluster containing the given position
    for (;;)
    {
		plane = node->plane;

		if (plane->type < 3)
			dist = pos[plane->type] - plane->dist;
		else 
			dist = DotProduct(pos, plane->normal) - plane->dist;
			
		if (dist > 0)
		{
			if (node->children[0] < 0)
			{
				leaf = ~node->children[0];
				break;
			}
			else
				node = &cm.nodes[node->children[0]];
		}
		else
		{
			if (node->children[1] < 0)
			{
				leaf = ~node->children[1];
				break;
			}
			else
				node = &cm.nodes[node->children[1]];
		}
    }
	
    if (leaf >= 0)
		cluster = cm.leaves[leaf].cluster;

    return cluster;
}

static int face_cmp(const void *a, const void *b)
{
    return ((rendface_t*)a)->sortkey - ((rendface_t*)b)->sortkey;
}

static void sort_faces(void)
{
    /* FIXME: expand qsort code here to avoid function calls */
    qsort((void*)facelist.faces, facelist.numfaces, sizeof(rendface_t),
	  face_cmp);
}

