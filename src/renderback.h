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
#ifndef __RENDER_BACK_H__
#define __RENDER_BACK_H__

#define MAX_ARRAYS_VERTS 4096
#define MAX_ARRAYS_ELEMS MAX_ARRAYS_VERTS * 3 

typedef struct
{
    int face;
    uint_t sortkey;
} rendface_t;

// List of faces to render
typedef struct
{
    int numfaces;
    rendface_t *faces;
} facelist_t;

void R_BackendFinalize(void);
void Render_Backend(facelist_t *list);
void R_BackendSky(int numsky, int *skylist);
void R_BackendMapent(int mapent);
void R_BackendFlush (int shadernum, int lmtex);
void R_BackendInit(void);
void R_BackendShutdown(void);
void R_BackendOverlay (quad_t * q, int numquads);
void R_BackendPushQuad (quad_t *quad);
void R_BackendPushPoly (poly_t *p);
void R_BackendClear( void );
void R_BackendClearColor( void );
void R_BackendClearEntityColor( void );
void R_BackendSetEntityColor( const colour_t c );
void R_BackendPushRaw( vec3_t *v, vec3_t *n, vec2_t *tc, int *elems, int numverts, int numelems );
void R_BackendInterpolateNormals( vec3_t *n1, vec3_t *n2, float frac, int numverts );

#endif /*__RENDER_BACK_H__*/
