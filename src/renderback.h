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

#include "a_shared.h"


enum {
	REND_TYPE_FACE,
	REND_TYPE_QUAD,
	REND_TYPE_MD3_MESH,
	REND_TYPE_POLY
};


typedef struct
{
    int face;
	int type ;
    uint_t sortkey;
} rendface_t;

/* List of faces to render */
typedef struct
{
    int numfaces;
    rendface_t *faces;
} facelist_t;





void render_backend_finalize(void);
void render_backend(facelist_t *list);
void render_backend_sky(int numsky, int *skylist);
void render_backend_mapent(int mapent);


extern void (* R_backend_flush  ) (int shader ,int lightmap );
void R_backend_init(void);
void R_render_model (const refEntity_t *re);
void R_Push_raw (vec3_t * v,vec2_t *tc ,colour_t *c,int * elems ,int numverts ,int numelems );
void R_backend_shutdown(void );
void Render_backend_Overlay ( quad_t * q,int numquads );
/* Preferentially sort by shader number, then lightmap */
/* FIXME: other things that could go in the sort key include transparency
 * and 'sort' directives from the shader scripts */
//#define SORTKEY(face)  ( ((face)->shadernum << 16) + (face)->lm_texnum+1)

#include "vec.h"
typedef struct
{
    int numverts;
    vec3_t *verts;
	vec3_t * norms ;
    colour_t *colour;
	colour_t *entity_colour;
    texcoord_t *tex_st;
    texcoord_t *lm_st;
	vec2_t ** stage_tex_st ;
	colour_t * mod_colour;
    int numelems;
    int *elems;
} arrays_t;










extern arrays_t arrays;


#endif /*__RENDER_BACK_H__*/
