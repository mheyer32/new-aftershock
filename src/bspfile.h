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


#ifndef BSP_FILE_H__
#define BSP_FILE_H__

#define BSPHEADER_ID  (*(int*)"IBSP")
#define BSPHEADER_VERSION 46

#define	LUMP_ENTITIES		0
#define	LUMP_SHADERS		1
#define	LUMP_PLANES			2
#define	LUMP_NODES			3
#define	LUMP_LEAFS			4
#define	LUMP_LEAFSURFACES	5
#define	LUMP_LEAFBRUSHES	6
#define	LUMP_MODELS			7
#define	LUMP_BRUSHES		8
#define	LUMP_BRUSHSIDES		9
#define	LUMP_DRAWVERTS		10
#define	LUMP_DRAWINDEXES	11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

typedef struct 
{
	int fileofs;
	int filelen;
} lump_t;

typedef struct {
	int id, ver;
	lump_t lumps[HEADER_LUMPS];
} bsp_header_t;


typedef struct
{
    char name[MAX_APATH];
    int flags;
	int contents;
} dshaderref_t;

// Face planes
typedef struct
{
    vec3_t vec;    // Normal to plane
    float offset;  // Distance to plane along normal
} dplane_t;

typedef struct
{
    int plane;        // Dividing plane
    int children[2];  // Left and right node.  Negatives are leafs
    int mins[3];
	int maxs[3];
} dnode_t;

// Leafs in BSP tree
typedef struct
{
    int cluster;    // Visibility cluster number
    int area;
    int mins[3];
	int maxs[3];
    int firstface, numfaces;
    int firstbrush, numbrushes;
} dleaf_t;

// Model 0 is the main map, others are doors, gates, buttons, etc.
typedef struct
{
    vec3_t mins;
	vec3_t maxs;
    int firstface, numfaces;
    int firstbrush, numbrushes;
} dmodel_t;


typedef struct
{
	int firstside; // Points to lump 10
	int numsides;
	int shader;
} dbrush_t; 

typedef struct
{
	int plane; // Points to plane_t
	int shader;
} dbrushside_t; 

typedef struct
{
    vec3_t v_point;     // World coords
    vec2_t tex_st;		// Texture coords
	vec2_t lm_st;		// Lightmap texture coords
    vec3_t v_norm;      // Normal
    colour_t colour;    // Colour used for vertex lighting?
} dvertex_t;

typedef struct
{ 
	char shadername[MAX_APATH];
	int brushnum; 
	int visibleside; 
} dfogzone_t; 

// Faces (or surfaces)
typedef struct
{
    int shader;     // Shader reference
    int fognum;
    int facetype;   // FACETYPE enum
    int firstvert, numverts;
    int firstelem, numelems;
    int lm_texnum;   // lightmap info
    int lm_offset[2];
    int lm_size[2];
    vec3_t v_orig;   // FACETYPE_NORMAL only
    vec3_t mins;
    vec3_t maxs;     // FACETYPE_MESH only
    vec3_t v_norm;   // FACETYPE_NORMAL only
    int mesh_cp[2];  // mesh control point dimensions
} dface_t;

// Potentially visible set (PVS) data
typedef struct
{
    int numclusters;   // Number of PVS clusters
    int rowsize;
    byte data[1];
} dvisibility_t;

#endif 