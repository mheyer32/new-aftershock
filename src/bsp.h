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
#ifndef __BSP_H__
#define __BSP_H__

#include "a_shared.h"

enum
{
    FACETYPE_NORMAL   = 1,
    FACETYPE_MESH     = 2,
    FACETYPE_TRISURF  = 3,
    FACETYPE_FLARE    = 4
};

//typedef int bbox_t[6];        /* Integer bounding box (mins, maxs)*/
//typedef float bboxf_t[6];     /* Float bounding box */
typedef float texcoord_t[2];  /* Texture s&t coordinates */
typedef byte colour_t[4];   /* RGBA */

/* Model 0 is the main map, others are doors, gates, buttons, etc. */
typedef struct
{
    vec3_t mins;
	vec3_t maxs;
    int firstface, numfaces;
    int firstbrush, numbrushes;
} dmodel_t;

/* Face planes */
typedef struct
{
    vec3_t vec;    /* Normal to plane */
    float offset;  /* Distance to plane along normal */
} dplane_t;



typedef cplane_t cplane_s;


void setSigns( cplane_t *in);


/* Nodes in the BSP tree */
typedef struct
{
    int plane;        /* Dividing plane */
    int children[2];  /* Left and right node.  Negatives are leafs */
    int mins[3];
	int maxs[3];
} dnode_t;

/* Leafs in BSP tree */
typedef struct
{
    int cluster;    /* Visibility cluster number */
    int area;       /* ? */
    int mins[3];
	int maxs[3];
    int firstface, numfaces;
    int firstbrush, numbrushes;
} dleaf_t;


/* Faces (or surfaces) */
typedef struct
{
    int shader;      /* Shader reference */
    int unknown;
    int facetype;   /* FACETYPE enum */
    int firstvert, numverts;
    int firstelem, numelems;
    int lm_texnum;    /* lightmap info */
    int lm_offset[2];
    int lm_size[2];
    vec3_t v_orig;   /* FACETYPE_NORMAL only */
    vec3_t mins;
    vec3_t maxs;     /* FACETYPE_MESH only */
    vec3_t v_norm;   /* FACETYPE_NORMAL only */
    int mesh_cp[2];  /* mesh control point dimensions */
} dface_t;




/* Shader references (indexed from faces) */
typedef struct
{
    char name[64];
    int flags;
	int contents;
} dshaderref_t;


typedef struct 
{
	int shader;
	int flags;
	int contents ;
}cshaderref_t ;

typedef struct
{
int firstside; /* Points to lump 10 */
int numsides;
int shader;
} dbrush_t; 

/* Lump 10: brush sides */
typedef struct
{
int plane; /* Points to plane_t */
int shader;
} dbrushside_t; 


/* Lump 13: fogzones */
typedef struct
{ 
char shadername[64];
int firstbrush; 
int numbrushes; 
} dfogzone_t; 




/* Vertex info */
typedef struct
{
    vec3_t v_point;     /* World coords */
    texcoord_t tex_st;  /* Texture coords */
    texcoord_t lm_st;   /* Lightmap texture coords */
    vec3_t v_norm;      /* Normal */
    colour_t colour;    /* Colour used for vertex lighting ? */
} vertex_t;

/* Potentially visible set (PVS) data */
typedef struct
{
    int numclusters;   /* Number of PVS clusters */
    int rowsize;
    byte data[1];
} visibility_t;


// TODO :make faster in-memory representations of these types 
// this doesn`t make a big sense a speedup can`t be seen

typedef struct
{
    cplane_t * plane ;
    int children[2];  /* Left and right node.  Negatives are leafs */
    vec3_t mins;
	vec3_t maxs;
} cnode_t;


typedef struct 
{
	cplane_t *plane;
	cshaderref_t *shader;
}cbrushside_t;

typedef struct {
	cbrushside_t *sides;
	int numsides;
	cshaderref_t * shader;
}cbrush_t ;


typedef struct
{
    vec3_t mins;
	vec3_t maxs;
    int firstface, numfaces;
	cbrush_t * brushes;
	int numbrushes;
} cmodel_t;

/* Leafs in BSP tree */
typedef struct
{
    int cluster;    /* Visibility cluster number */
    int area;       /* ? */
    vec3_t mins;
	vec3_t maxs;
    int firstface, numfaces;
    int firstbrush, numbrushes;
} cleaf_t;

typedef struct
{
    int shadernum;
    int unknown;
    int facetype;   /* FACETYPE enum */
    vertex_t * verts;
	int numverts;
    int		*elems; 
	int numelems;
    int lm_texnum;    /* lightmap info */
    int lm_offset[2];
    int lm_size[2];
    vec3_t v_orig;   /* FACETYPE_NORMAL only */
    vec3_t mins;
    vec3_t maxs;     /* FACETYPE_MESH only */
    vec3_t v_norm;   /* FACETYPE_NORMAL only */
    int mesh_cp[2];  /* mesh control point dimensions */
} cface_t;



extern int r_nummodels, r_numverts, r_numplanes, r_numleafs, r_numnodes;
extern int r_numshaders, r_numfaces, r_numlfaces, r_numelems;
extern int r_lightmapsize;
extern int r_addshaderstart;
extern float r_sinfov2;
extern float r_cosfov2;


typedef struct 
{
	char name [MAX_APATH];
	cshaderref_t * shaderrefs ;
	int *shadernums;
	cplane_t * planes ;
	cnode_t * nodes;
	cleaf_t * leafs ;
	int *lfaces ;
	int *lbrushes ;
	cmodel_t * models ;
	cbrush_t * brushes ;
	cbrushside_t * brushsides ;
	vertex_t * vertices;
	int * elems ;
// fog;
	cface_t * faces;
	int * lightmaps ;
// lightgrid ;
	visibility_t * vis ;

	int num_shaders ;
	int num_planes ;
	int num_nodes;
	int num_leafs ;
	int num_lfaces ;
	int num_lbrushes ;
	int num_models ;
	int num_brushes ;
	int num_brushsides ;
	int num_vertices ;
	int num_elems ;
// fog;
	int num_faces;
	int num_lightmap;
// lightgrid ;
	int num_vis ;

	int r_data_loaded;
	

}map_t;


extern map_t map ;


/* PVS test macro:  PVS table is a packed single bit array, rowsize
   bytes times numclusters rows */
#define BSP_TESTVIS(from,to) \
        (*(map.vis->data + (from)*map.vis->rowsize + \
           ((to)>>3)) & (1 << ((to) & 7)))

#endif /*__BSP_H__*/
