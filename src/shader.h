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
#ifndef __SHADER_H__
#define __SHADER_H__

#define SHADERPASS_MAX 5
#define SHADER_ANIM_FRAMES_MAX 8
#define SHADERBUF_SIZE  (1024*1024-8)
#define MAX_TC_MOD  8


#define SHADER_DOCULL 1<<1
/* Shader flags */
enum
{
    SHADER_NOCULL        = 1 << 0,
    SHADER_TRANSPARENT   = 1 << 1,
    SHADER_DEPTHWRITE    = 1 << 2,  /* Also used for pass flag */
    SHADER_SKY           = 1 << 3,
    SHADER_NOMIPMAPS     = 1 << 4,
    SHADER_NEEDCOLOURS   = 1 << 5,
    SHADER_DEFORMVERTS   = 1 << 6,
	SHADER_POLYGONOFFSET = 1 << 7,
	SHADER_FOG           = 1 << 8,
	SHADER_MULTITEXTURE  = 1 << 9,
	SHADER_NOPICMIP      = 1 << 10,
	SHADER_CLAMP         = 1 << 11  // will just be used for texture loading 
};

/* Shaderpass flags */
enum
{
    SHADER_LIGHTMAP   = 1 << 0,
    SHADER_BLEND      = 1 << 1,
    SHADER_ALPHAFUNC  = 1 << 3,
    SHADER_TCMOD      = 1 << 4,
    SHADER_ANIMMAP    = 1 << 5,
};	

/* Transform functions */
enum
{
    SHADER_FUNC_SIN             = 1,
    SHADER_FUNC_TRIANGLE        = 2,
    SHADER_FUNC_SQUARE          = 3,
    SHADER_FUNC_SAWTOOTH        = 4,
    SHADER_FUNC_INVERSESAWTOOTH = 5,

};

/* tcmod functions */
enum
{
    SHADER_TCMOD_SCALE               = 1 << 1,
    SHADER_TCMOD_ROTATE              = 1 << 2,
    SHADER_TCMOD_SCROLL              = 1 << 3,
    SHADER_TCMOD_TRANSFORM           = 1 << 4,
    SHADER_TCMOD_TURB                = 1 << 5,
    SHADER_TCMOD_STRETCH             = 1 << 6
};

// SORTING 
enum {
	SHADER_SORT_NONE = 0,
	SHADER_SORT_PORTAL = 1,
	SHADER_SORT_SKY = 2,
	SHADER_SORT_OPAQUE = 3,
	SHADER_SORT_BANNER = 6,
	SHADER_SORT_UNDERWATER = 8,
	SHADER_SORT_ADDITIVE = 9,
	SHADER_SORT_NEAREST = 16
} ;


// RGB_Gen :

enum {
	RGB_GEN_IDENTITY_LIGHTING = 0,
	RGB_GEN_IDENTITY=1,
	RGB_GEN_WAVE =2,
	RGB_GEN_ENTITY =3,
	RGB_GEN_ONE_MINUS_ENTITY=4,
	RGB_GEN_VERTEX=5,
	RGB_GEN_ONE_MINUS_VERTEX=6,
	RGB_GEN_LIGHTING_DIFFUSE =7,
	RGB_GEN_EXACT_VERTEX =8
};


// ALPHA_GEN :

enum {
	ALPHA_GEN_DEFAULT =0,
	ALPHA_GEN_PORTAL =1,
	ALPHA_GEN_VERTEX ,
	ALPHA_GEN_ENTITY,
	ALPHA_GEN_LIGHTINGSPECULAR,
	ALPHA_GEN_WAVE // TODO !
};


// TC_GEN :

enum {
	TC_GEN_BASE =0,
	TC_GEN_LIGHTMAP =1,
	TC_GEN_ENVIRONMENT =2,
	TC_GEN_VECTOR =3,
};

// Deform_Vertices :

enum {
	DEFORMV_NONE,
	DEFORMV_WAVE,
	DEFORMV_NORMAL,
	DEFORMV_BULGE,
	DEFORMV_MOVE,
	DEFORMV_AUTOSPRITE,
	DEFORMV_AUTOSPRITE2
};



#define SHADER_BSP 0
#define SHADER_MD3 1
#define SHADER_2D  2





/* Periodic functions */
typedef struct
{
    uint_t func;     /* SHADER_FUNC enum */
    float args[4];   /* offset, amplitude, phase_offset, rate */
} shaderfunc_t;

typedef struct {
	int type ;
	float args[6];
} tc_mod_t;

/* Per-pass rendering state information */
typedef struct
{
    uint_t flags;
    int texref;                 /* Texture ref (if not lightmap) */
    uint_t blendsrc, blenddst;  /* glBlend args */
    uint_t depthfunc;           /* glDepthFunc arg */
    uint_t alphafunc;           /* glAlphaFunc arg1 */
    float alphafuncref;         /* glAlphaFunc arg2 */
    int   rgbgen;             
    shaderfunc_t rgbgen_func;
	int tc_gen ;
	float tc_gen_args [6]; // if tc_gen == VECTOR ;

    int  num_tc_mod;               
	tc_mod_t tc_mod [ MAX_TC_MOD];
	shaderfunc_t tc_mod_stretch;

	int alpha_gen ;

    float anim_fps;             /* Animation frames per sec */
    int anim_numframes;
    int anim_frames[SHADER_ANIM_FRAMES_MAX];  /* Texture refs */
} shaderpass_t;

/* Shader info */
typedef struct
{	
	char name [64];
    uint_t flags;
	int contents;
    int numpasses;
	int portalstate ; // TEST 
	int sort ;
	int deform_vertices;
    shaderpass_t pass[SHADERPASS_MAX];
    float skyheight;          /* Height for skybox */
    float deform_params[4];
	float fog_params [4];
    shaderfunc_t deformv_wavefunc;
} shader_t;

/* Special texture loading requirements */
enum
{
    TEXFILE_NOMIPMAPS  = 1 << 0,
    TEXFILE_CLAMP      = 1 << 1,
	TEXFILE_NOSCALEDOWN= 1 << 2
};

typedef struct
{
    char *keyword;
    int minargs, maxargs;
    void (* func)(shader_t *shader, shaderpass_t *pass,
		  int numargs, char **args);
} shaderkey_t;


#define MAX_NUM_TEXTURES 512
#define SHADER_ARGS_MAX (SHADER_ANIM_FRAMES_MAX+1)
#define LOWERCASE(c) ((c) <= 'Z' && (c) >= 'A' ? (c) + ('a'-'A') : (c))

/* Gathers texture file names prior to texture loading */
typedef struct
{
    uint_t flags;
    char *fname;
} texfile_t;


typedef struct
{
	int Flags;
	int id;
    char name[MAX_APATH];
} texture_t;


extern texture_t * r_dynamic_tex;

extern int r_numtextures;
extern shader_t *r_shaders;
extern texfile_t *r_texfiles;


extern int shader_white ;
extern int shader_text ;
extern int shader_console ;

int Shader_Init (void);
int Shader_Shutdown (void);



int  R_RegisterShaderNoMip( const char *name ) ;
int R_RegisterShader ( const char * name );

int R_LoadShader ( const char * name ,int type );

#endif /*__SHADER_H__*/
