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
#ifndef CMAP_H__
#define CMAP_H__

#include "a_shared.h"
#include "shader.h"

enum
{
    FACETYPE_NORMAL   = 1,
    FACETYPE_MESH     = 2,
    FACETYPE_TRISURF  = 3,
    FACETYPE_FLARE    = 4
};

typedef struct 
{
	int shadernum; // don`t use pointer: for sorting  
	int flags;
	int contents;
} cshaderref_t;

// cplane_t is already defined:
typedef struct 
{
	cplane_t *plane;
	cshaderref_t *shaderref;
} cbrushside_t;

typedef struct 
{
	cplane_t *plane;
	int children[2];
	vec3_t mins;
	vec3_t maxs;
} cnode_t;

typedef struct 
{
	int cluster;
	int area;
	vec3_t mins;
	vec3_t maxs;
	int firstface;
	int numfaces;
	int firstbrush;
	int numbrushes;
} cleaf_t;

typedef struct 
{
	cbrushside_t *sides;
	int numsides;
	cshaderref_t *shaderref;
} cbrush_t;

typedef struct
{
    vec3_t v_point;			// World coords
    vec2_t tex_st;			// Texture coords
	vec2_t lm_st;			// Lightmap texture coords
    vec3_t v_norm;			// Normal
    colour_t colour;		// Colour used for vertex lighting?
} cvertex_t;

typedef struct 
{
	int shadernum;
	int unknown;
	int facetype;
	cvertex_t *verts;
	int numverts;
	int *elems;
	int numelems;
	int lightmapnum;
	int lm_offset[2]; // TODO 
	int lm_size[2];   // TODO
	vec3_t v_orig;
	vec3_t mins;
	vec3_t maxs;
	vec3_t normal;
	int mesh_cp[2];
} cface_t;

typedef struct 
{
	vec3_t mins;
	vec3_t maxs;
	cface_t *faces;
	int numfaces;
	cbrush_t *brushes;
	int numbrushes;
} cmodel_t;

typedef struct 
{
	shader_t *shader;
	cbrush_t *brushes;
	int numbrushes;
} cfogzone_t;


// TODO !!!
// lightgrid !!!

// Potentially visible set (PVS) data
typedef struct
{
    int numclusters;   // Number of PVS clusters
    int rowsize;
    byte data[1];
} cvisibility_t;

typedef struct {
	char			*entityspawn;
	cshaderref_t	*shaderrefs;
	cplane_t		*planes;
	cnode_t			*nodes;
	cleaf_t			*leaves;
	int				*lfaces;
	int				*lbrushes;
	cmodel_t		*models;
	cbrush_t		*brushes;
	cbrushside_t	*brushsides;
	cfogzone_t		*fog;
	cvertex_t		*vertices;
	int             *elems;
	cface_t			*faces;
	byte			*lightmapdata;
	cvisibility_t	*vis;
	// lightgrid !!!!

	int				num_shaderrefs;
	int				num_planes;
	int				num_nodes;
	int				num_leaves;
	int				num_lfaces;
	int				num_lbrushes;
	int				num_models;
	int				num_brushes;
	int				num_brushsides;
	int				num_fog;
	int				num_vertices;
	int				num_elems;
	int				num_faces;
	int				lightmapdata_size;
	// lightgrid !!!!

	// management stuff;
	char			name[MAX_APATH];
	aboolean		r_data_loaded;
} cmap_t;

extern cmap_t cm;

/* PVS test macro:  PVS table is a packed single bit array, rowsize
   bytes times numclusters rows */
#define BSP_TESTVIS(from, to) \
        (*(cm.vis->data + (from)*cm.vis->rowsize + \
           ((to) >> 3)) & (1 << ((to) & 7)))

aboolean CM_LoadMap (const char *mapname, aboolean load_rdata);
void CM_FreeMap (void);

#endif 