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
#ifndef __RENDER_H__
#define __RENDER_H__

extern vec3_t r_eyepos;      /* World coords */
extern vec3_t r_eyedir;      /* Camera direction unit vector */
extern float r_eye_az, r_eye_el;  /* eyedir azimuth and elevation (deg) */
extern float r_eyefov;       /* Field of view angle (deg) */
extern float r_sinfov;
extern float r_cosfov;
extern float r_tanfov;
extern float r_aspect;
extern int r_eyecluster;     /* Current PVS cluster */
extern float g_frametime;   /* Current time */


#include "c_var.h"


/* cvars */
extern cvar_t *con_notifytime;
extern cvar_t *r_allowExtensions;
extern cvar_t *r_allowSoftwareGL;
extern cvar_t *r_clear;
extern cvar_t *r_colorbits;
extern cvar_t *r_colorMipLevels;
extern cvar_t *r_depthbits;
#ifdef _WIN32
extern cvar_t *r_displayrefresh;
#endif
extern cvar_t *r_drawBuffer;
extern cvar_t *r_drawentities;
extern cvar_t *r_drawworld;
extern cvar_t *r_dynamiclight;
extern cvar_t *r_ext_compiled_vertex_array;
extern cvar_t *r_ext_compress_textures;
#ifdef _WIN32
extern cvar_t *r_ext_gamma_control;
#endif
extern cvar_t *r_ext_multitexture;
#ifndef __linux__
extern cvar_t *r_ext_swap_control;
#endif
extern cvar_t *r_ext_texture_env_add;
extern cvar_t *r_facePlaneCull;
extern cvar_t *r_fastsky;
extern cvar_t *r_finish;
extern cvar_t *r_fullbright;
extern cvar_t *r_fullscreen;
extern cvar_t *r_gamma;
extern cvar_t *r_glDriver;
extern cvar_t *r_ignoreFastPath;
extern cvar_t *r_ignoreGLErrors;
extern cvar_t *r_lastValidRenderer;
extern cvar_t *r_lightmap;
extern cvar_t *r_lockpvs;
extern cvar_t *r_lodbias;
extern cvar_t *r_logFile;
extern cvar_t *r_mapOverBrightBits;
extern cvar_t *r_mode;
extern cvar_t *r_nocurves;
extern cvar_t *r_offsetfactor;
extern cvar_t *r_offsetunits;
extern cvar_t *r_overBrightBits;
extern cvar_t *r_picmip;
extern cvar_t *r_primitives;
extern cvar_t *r_printShaders;
extern cvar_t *r_roundImagesDown;
extern cvar_t *r_screenshot_format;
extern cvar_t *r_showcluster;
extern cvar_t *r_showImages;
extern cvar_t *r_shownormals;
extern cvar_t *r_showsky;
extern cvar_t *r_showtris;
extern cvar_t *r_speeds;
extern cvar_t *r_stencilbits;
extern cvar_t *r_stereo;
extern cvar_t *r_subdivisions;
extern cvar_t *r_swapInterval;
extern cvar_t *r_texturebits;
extern cvar_t *r_textureMode;
extern cvar_t *r_verbose;
extern cvar_t *r_vertexLight;
extern cvar_t *r_znear;
extern cvar_t *vid_xpos;
extern cvar_t *vid_ypos;
extern cvar_t *win_wndproc;




#include "vec.h"
#define	MAX_RENDER_STRINGS			8
#define	MAX_RENDER_STRING_LENGTH	32



#define	MAX_DLIGHTS		32			// can't be increased, because bit flags are used on surfaces
#define	MAX_ENTITIES	1023		// can't be increased without changing drawsurf bit packing

// renderfx flags
#define	RF_MINLIGHT			1		// allways have some light (viewmodel, some items)
#define	RF_THIRD_PERSON		2		// don't draw through eyes, only mirrors (player bodies, chat sprites)
#define	RF_FIRST_PERSON		4		// only draw through eyes (view weapon, damage blood blob)
#define	RF_DEPTHHACK		8		// for view weapon Z crunching
#define	RF_NOSHADOW			64		// don't add stencil shadows

#define RF_LIGHTING_ORIGIN	128		// use refEntity->lightingOrigin instead of refEntity->origin
									// for lighting.  This allows entities to sink into the floor
									// with their origin going solid, and allows all parts of a
									// player to get the same lighting
#define	RF_SHADOW_PLANE		256		// use refEntity->shadowPlane
#define	RF_WRAP_FRAMES		512		// mod the model frames by the maxframes to allow continuous
									// animation without needing to know the frame count

// refdef flags
#define RDF_NOWORLDMODEL	1		// used for player configuration screen
#define RDF_HYPERSPACE		4		// teleportation effect



typedef struct {
	int			x, y, width, height;
	float		fov_x, fov_y;
	vec3_t		vieworg;
	vec3_t		viewaxis[3];		// transformation matrix

	// time in milliseconds for shader effects and other time dependent rendering issues
	int			time;

	int			rdflags;			// RDF_NOWORLDMODEL, etc

	// 1 bits will prevent the associated area from rendering at all
	byte		areamask[32];

	// text messages for deform text shaders
	char		text[MAX_RENDER_STRINGS][MAX_RENDER_STRING_LENGTH];
} refdef_t;



typedef enum {
	RT_MODEL,
	RT_POLY,
	RT_SPRITE,
	RT_BEAM,
	RT_RAIL_CORE,
	RT_RAIL_RINGS,
	RT_LIGHTNING,
	RT_PORTALSURFACE,		// doesn't draw anything, just info for portals

	RT_MAX_REF_ENTITY_TYPE
} refEntityType_t;

typedef struct {
	refEntityType_t	reType;
	int			renderfx;

	int     	hModel;				// opaque type outside refresh

	// most recent data
	vec3_t		lightingOrigin;		// so multi-part models can be lit identically (RF_LIGHTING_ORIGIN)
	float		shadowPlane;		// projection shadows go here, stencils go slightly lower

	vec3_t		axis[3];			// rotation vectors
	int     	nonNormalizedAxes;	// axis are not normalized, i.e. they have scale
	float		origin[3];			// also used as MODEL_BEAM's "from"
	int			frame;				// also used as MODEL_BEAM's diameter

	// previous data for frame interpolation
	float		oldorigin[3];		// also used as MODEL_BEAM's "to"
	int			oldframe;
	float		backlerp;			// 0.0 = current, 1.0 = old

	// texturing
	int			skinNum;			// inline skin index
	int     	customSkin;			// NULL for default skin
	int     	customShader;		// use one image for the entire thing

	// misc
	byte		shaderRGBA[4];		// colors used by rgbgen entity shaders
	float		shaderTexCoord[2];	// texture coordinates used by tcMod entity modifiers
	float		shaderTime;			// subtracted from refdef time to control effect start times

	// extra sprite information
	float		radius;
	float		rotation;
} refEntity_t;

typedef struct {
	vec3_t		xyz;
	float		st[2];
	byte		modulate[4];
} polyVert_t;

typedef struct poly_s {
	int     			hShader;
	int					numVerts;
	polyVert_t			*verts;
} poly_t;



// used for overlay :
typedef struct 
{

	int shader;
	int elems[6];
	vec3_t verts [4];
	float tc [4][2];
	colour_t color [4];

}quad_t;


typedef struct {
	vec3_t origin ;
	float intensity;
	float r,g,b;
}dlight_t;


// used for everything which has its own coordinate system :
typedef struct 
{
	vec3_t matrix[3];
	vec3_t inv_matrix [3];
	vec3_t pos;
	
	aboolean matrix_identity;
	aboolean pos_identity;
	aboolean inv_matrix_calculated;
}reference_t;


/*
#include <windows.h>


extern HINSTANCE hInst;
extern HDC dc;
extern HWND hWnd;
extern HGLRC hRC;
*/


void R_StartFrame (void );
void R_EndFrame (void );



void R_DrawStretchPic( float x,float y,float w,float h,float s1,float t1,float s2,float t2,int hShader);
void R_SetColor( const float *rgba );
void R_Init(void );
void R_Shutdown(void );
void R_DrawString( int x, int y, const char *str, vec4_t color );
void R_AddPolyToScene(  const polyVert_t *verts , int numVerts, int Shader ) ;
void R_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b );
void R_Update_Screen(void );
void R_ClearScene (void );
void R_RenderScene (const refdef_t * fd );
void R_AddRefEntityToScene( const refEntity_t *re ) ;
int R_TestVis ( const vec3_t p1, const vec3_t p2 );

int R_RegisterModel( const char *name ) ;
int  R_RegisterSkin( const char *name );




#endif /*__RENDER_H__*/
