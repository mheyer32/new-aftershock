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
#ifndef __MD3_H__
#define __MD3_H__

#include "a_shared.h"

#define MD3_ID_HEADER	 (*(int*)"IDP3")
#define MD3_ALIAS_VERSION	15
#define MAX_MD3_MESHES      16
#define MAX_SKINS			MAX_MODELS << 2

// vertex scales
#define	MD3_XYZ_SCALE		(1.0/64)

typedef struct
{
	char			name[MAX_APATH];
	int				flags;
    int				shader;		// Shader reference
    int				numverts;
	int				numskins;
	int				numtris;
	int				numframes;
    vec3_t			**points;
	vec3_t			*normals;
    vec2_t			*tex_st;	// Texture coords
    vec3_t			**norms;	// Used for environment mapping?
    int				numelems;
    uint_t			*elems;
	int				*skins;
} md3mesh_t;

typedef struct
{
    vec3_t			mins;
	vec3_t			maxs;
    int				nummeshes;
    md3mesh_t		*meshes;
} md3model_t;

typedef struct
{
    vec3_t			mins;
	vec3_t			maxs;
    vec3_t			pos;
    float			scale;
    char			creator[16];
} md3boneframe_t;

typedef struct
{
    int				id;
    int				version;
    char			filename[MAX_APATH];
	int				flags;
    int				numboneframes;
    int				numtags;
    int				nummeshes;
    int				numskins;
    int				bone_offs;
    int				tag_offs;
    int				mesh_offs;
    int				filesize;
} md3header_t;

typedef struct
{
	char			name[MAX_APATH];	// tag name
	orientation_t	orient;
} md3tag_t;

typedef struct
{
    int				id;
    char			name[MAX_APATH];
	int				flags;
    int				numframes;
    int				numskins;
    int				numverts;
    int				numtris;
    int				elem_offs;
    int				skin_offs;
    int				tc_offs;
    int				vert_offs;
    int				meshsize;
} md3mesh_file_t;


typedef struct 
{
	char			name[MAX_OSPATH];
	vec3_t			mins, maxs;
	int				nummeshes;
	int				numframes;
	int				numtags;
	int				numskins;
	int				*ids;
	md3tag_t		**tags;
	md3mesh_t		*meshes;
	md3boneframe_t	*frames;
} md3model2_t;

typedef struct
{
    signed short	vec[3];
    byte			tc[2];
} md3vert_t;

typedef struct 
{
	char			mesh_name[MAX_APATH];
	int				shaderref;
} mesh_skin_t;

typedef struct {
	char			name[MAX_APATH];
	mesh_skin_t		skins[MAX_MD3_MESHES];
	int				num_mesh_skins;
} skin_t;

void MD3_Shutdown (void);
void MD3_Init(void);
aboolean R_LoadMD3(md3model2_t *md3, const char *filename);

extern md3model2_t r_md3models[MAX_MODELS];
extern skin_t md3skins[MAX_SKINS];
extern int r_md3Skincount;
extern int r_md3Modelcount;

#endif /*__MD3_H__*/
