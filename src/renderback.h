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

#define MAX_ARRAYS_VERTS 8192
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

void Render_Backend_Finalize(void);
void Render_Backend(facelist_t *list);
void Render_Backend_Sky(int numsky, int *skylist);
void Render_Backend_Mapent(int mapent);
void Render_Backend_Flush (int shadernum, int lmtex);
void Render_Backend_Init(void);
void Render_Push_Raw (vec3_t *v, vec2_t *tc, colour_t *c, int *elems, int numverts, int numelems);
void Render_Backend_Shutdown(void);
void Render_Backend_Overlay (quad_t * q, int numquads);

extern arrays_t arrays;

#endif /*__RENDER_BACK_H__*/
