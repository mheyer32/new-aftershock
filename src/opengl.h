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


#ifndef OPENGL__H__
#define OPENGL__H__


#include "glinc.h"

typedef enum {
	TC_NONE,
	TC_S3TC
} textureCompression_t;

typedef enum {
	GLDRV_ICD,					// driver is integrated with window system
								// WARNING: there are tests that check for
								// > GLDRV_ICD for minidriverness, so this
								// should always be the lowest value in this
								// enum set
	GLDRV_STANDALONE,			// driver is a non-3Dfx standalone driver
	GLDRV_VOODOO				// driver is a 3Dfx standalone driver
} glDriverType_t;

typedef enum {
	GLHW_GENERIC,			// where everthing works the way it should
	GLHW_3DFX_2D3D,			// Voodoo Banshee or Voodoo3, relevant since if this is
							// the hardware type then there can NOT exist a secondary
							// display adapter
	GLHW_RIVA128,			// where you can't interpolate alpha
	GLHW_RAGEPRO,			// where you can't modulate alpha on alpha textures
	GLHW_PERMEDIA2			// where you don't have src*dst
} glHardwareType_t;





typedef struct {
	char					renderer_string[MAX_STRING_CHARS];
	char					vendor_string[MAX_STRING_CHARS];
	char					version_string[MAX_STRING_CHARS];
	char					extensions_string[MAX_STRING_CHARS];

	int						maxTextureSize;			// queried from GL
	int						maxActiveTextures;		// multitexture ability

	int						colorBits, depthBits, stencilBits;

	glDriverType_t			driverType;
	glHardwareType_t		hardwareType;

	aboolean				deviceSupportsGamma;
	textureCompression_t	textureCompression;
	aboolean				textureEnvAddAvailable;

	int						vidWidth, vidHeight;
	// aspect is the screen's physical width / height, which may be different
	// than scrWidth / scrHeight if the pixels are non-square
	// normal screens should be 4/3, but wide aspect monitors may be 16/9
	float					windowAspect;

	int						displayFrequency;

	// synonymous with "does rendering consume the entire screen?", therefore
	// a Voodoo or Voodoo2 will have this set to TRUE, as will a Win32 ICD that
	// used CDS.
	aboolean				isFullscreen;
	aboolean				stereoEnabled;
	aboolean				smpActive;		// dual processor
} glconfig_t;



extern glconfig_t glconfig;

typedef struct {

	int _TexEnv_Add;
	int _TexEnv_Combine;
	int _TexEnv_Combine4;
	int _GL_NV_register_combiners;
	int _ARB_Multitexture;
	int _CompiledVertex_Arrays;
	int _WGL_ARB_extensions_string;
	int _WGL_3DFX_gamma;
	int _WGL_swap_control;
	int _GL_ARB_texture_compression;
	int _GL_3DFX_texture_compression_FXT1;
	int _GL_S3_s3tc ;
}ext_info_t;


typedef struct {
	int width ;
	int height;
}vid_mode_t ;



extern ext_info_t gl_ext_info;

extern int winX;
extern int winY;
extern int Fullsreen;
extern int ColorBits;
extern int StencilBits;
extern int DisplayFrequency;
extern int DepthBits;

extern HINSTANCE hInst;
extern HDC dc;
extern HWND hWnd;
extern HGLRC hRC;
extern int nCmdShow;




void  GetGlConfig(glconfig_t * config);
int Init_OpenGL ( void );
int Shutdown_OpenGL (void );



void GL_Enable (int param );
void GL_Disable (int param );
void GL_ActiveTextureARB (int param );
void GL_DepthMask ( GLboolean state );
void GL_BindTexture (int par1,int id );
void GL_AlphaFunc(int func, float ref );
void GL_ClientActiveTextureARB (int par );
void GL_DisableClientState (int par );
void GL_EnableClientState (int par );
void GL_BlendFunc (int src, int dst );
void GL_DepthFunc (int func );

void GL_EnableTexUnitARB (int num );
void GL_DisableTexUnitARB (int num );

void GL_TexEnvf (int target ,int pname ,float param );



// GLU - Replacement :
GLint APIENTRY GL_ScaleImage( GLenum format,
                              GLsizei widthin, GLsizei heightin,
                              GLenum typein, const void *datain,
                              GLsizei widthout, GLsizei heightout,
                              GLenum typeout, void *dataout );

GLint APIENTRY GL_Build2DMipmaps( GLenum target, GLint components,
                                  GLsizei width, GLsizei height, GLenum format,
                                  GLenum type, const void *data );


void APIENTRY GL_Perspective( GLdouble fovy, GLdouble aspect,
                              GLdouble zNear, GLdouble zFar );


enum
{
 CPU_AMD = 1,
CPU_INTEL =2,
CPU_CYRIX =3,
CPU_CENTAUR =4, 
CPU_UNKNOWN =5
};










#endif 