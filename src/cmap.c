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

#include "a_shared.h"
#include "io.h"
#include "console.h"
#include "shader.h"
#include "bspfile.h"
#include "cmap.h"

static void *bspdata = NULL;
static bsp_header_t *header = NULL;
cmap_t cm;

static int CM_ReadLump (int lump, void **data, int elem)
{
	int len = header->lumps[lump].filelen;
	int num = len / elem;

	*data = malloc (len);

	memcpy (*data, (byte *)bspdata + header->lumps[lump].fileofs, num * elem);
	return num;
}

static void CM_Load_Shaderrefs (aboolean load_rdata)
{
	int i;
	dshaderref_t *shaders;
	 
	cm.num_shaderrefs = CM_ReadLump(SHADERREFS, &shaders, sizeof (dshaderref_t));
	cm.shaderrefs = malloc (cm.num_shaderrefs * sizeof (cshaderref_t));

	for (i = 0; i < cm.num_shaderrefs; i++)
	{
		cm.shaderrefs[i].contents = LittleLong (shaders[i].contents);
		cm.shaderrefs[i].flags = LittleLong (shaders[i].flags);
		
		if (load_rdata)
			cm.shaderrefs[i].shadernum = R_LoadShader (shaders[i].name, SHADER_BSP); 
	}

	free (shaders);
}

static void CM_Load_Planes (void)
{
	int i;
	dplane_t *planes;

	cm.num_planes = CM_ReadLump (DPLANES, &planes, sizeof (dplane_t ));
	cm.planes = malloc (cm.num_planes * sizeof (cplane_t ));

	for (i = 0; i < cm.num_planes; i++ )
	{
		cm.planes[i].dist = LittleFloat (planes[i].offset);
		cm.planes[i].normal[0] = LittleFloat (planes[i].vec[0]);
		cm.planes[i].normal[1] = LittleFloat (planes[i].vec[1]);
		cm.planes[i].normal[2] = LittleFloat (planes[i].vec[2]);
		cm.planes[i].type = PlaneTypeForNormal (cm.planes[i].normal);
		
		SetPlaneSignbits (&cm.planes[i]);
	}

	free (planes);
}

static void CM_Load_Nodes (void)
{
	int i;
	dnode_t *nodes;

	cm.num_nodes = CM_ReadLump (NODES, &nodes, sizeof(dnode_t));
	cm.nodes = malloc (cm.num_nodes * sizeof (cnode_t));

	for (i = 0; i < cm.num_nodes; i++ )
	{
		cm.nodes[i].children[0] = LittleLong (nodes[i].children[0]);
		cm.nodes[i].children[1] = LittleLong (nodes[i].children[1]);

		cm.nodes[i].mins[0] = (float)LittleLong (nodes[i].mins[0]);
		cm.nodes[i].mins[1] = (float)LittleLong (nodes[i].mins[1]);
		cm.nodes[i].mins[2] = (float)LittleLong (nodes[i].mins[2]);

		cm.nodes[i].maxs[0] = (float)LittleLong (nodes[i].maxs[0]);
		cm.nodes[i].maxs[1] = (float)LittleLong (nodes[i].maxs[1]);
		cm.nodes[i].maxs[2] = (float)LittleLong (nodes[i].maxs[2]);

		cm.nodes[i].plane = &cm.planes [LittleLong (nodes[i].plane)];
	}

	free (nodes);
}

static void CM_Load_Leaves (void)
{
	int i;
	dleaf_t *leafs;

	cm.num_leaves = CM_ReadLump (LEAFS, &leafs, sizeof (dleaf_t));
	cm.leaves = malloc (cm.num_leaves * sizeof (cleaf_t));

	for (i = 0; i < cm.num_leaves; i++ )
	{
		cm.leaves[i].area = LittleLong (leafs[i].area);
		cm.leaves[i].cluster = LittleLong (leafs[i].area);
		cm.leaves[i].firstbrush = LittleLong (leafs[i].firstbrush);
		cm.leaves[i].firstface = LittleLong (leafs[i].firstface);

		cm.leaves[i].mins[0] = (float)LittleLong (leafs[i].mins[0]);
		cm.leaves[i].mins[1] = (float)LittleLong (leafs[i].mins[1]);
		cm.leaves[i].mins[2] = (float)LittleLong (leafs[i].mins[2]);

		cm.leaves[i].maxs[0] = (float)LittleLong (leafs[i].maxs[0]);
		cm.leaves[i].maxs[1] = (float)LittleLong (leafs[i].maxs[1]);
		cm.leaves[i].maxs[2] = (float)LittleLong (leafs[i].maxs[2]);

		cm.leaves[i].numbrushes = LittleLong (leafs [i].numbrushes);
		cm.leaves[i].numfaces = LittleLong (leafs[i].numfaces);
	}

	free (leafs);
}

static void CM_Load_LFaces (aboolean load_rdata)
{
	int i;

	if (load_rdata)
	{
		cm.num_lfaces = CM_ReadLump (LFACES, &cm.lfaces, sizeof(int));

		for (i = 0; i < cm.num_lfaces; i++)
			cm.lfaces[i] = LittleLong (cm.lfaces[i]);
	}
}

static void CM_Load_LBrushes(void)
{
	int i;

	cm.num_lbrushes = CM_ReadLump (BRUSH_LIST, &cm.lbrushes, sizeof(int));

	for (i = 0; i < cm.num_lbrushes; i++)
		cm.lbrushes[i] = LittleLong (cm.lbrushes[i]);
}

static void CM_Load_Models (aboolean load_rdata)
{
	int i;
	dmodel_t *models;

	cm.num_models = CM_ReadLump (MODELS, &models, sizeof (dmodel_t));
	cm.models = malloc (cm.num_models * sizeof (cmodel_t));

	for (i = 0; i < cm.num_models; i++)
	{
		cm.models[i].mins[0] = LittleFloat (models[i].mins[0]);	
		cm.models[i].mins[1] = LittleFloat (models[i].mins[1]);
		cm.models[i].mins[2] = LittleFloat (models[i].mins[2]);

		cm.models[i].maxs[0] = LittleFloat (models[i].maxs[0]);
		cm.models[i].maxs[1] = LittleFloat (models[i].maxs[1]);
		cm.models[i].maxs[2] = LittleFloat (models[i].maxs[2]);

		cm.models[i].brushes = &cm.brushes[LittleLong (models[i].firstbrush)];
		cm.models[i].numbrushes = LittleLong (models[i].numbrushes);

		if (load_rdata)
		{
			cm.models[i].faces = &cm.faces[LittleLong (models[i].firstface)];
			cm.models[i].numfaces = LittleLong (models[i].numfaces);
		}
	}
	
	free (models);
}

static void CM_Load_Brushes (void)
{
	int i;
	dbrush_t *brushes;

	cm.num_brushes = CM_ReadLump (BRUSHES, &brushes, sizeof (dbrush_t));
	cm.brushes = malloc ( cm.num_brushes * sizeof (cbrush_t)); 
	
	for (i = 0; i < cm.num_brushes; i++ )
	{
		cm.brushes[i].shaderref = &cm.shaderrefs[LittleLong (brushes[i].shader)];
		cm.brushes[i].numsides = LittleLong (brushes[i].numsides);
		cm.brushes[i].sides = &cm.brushsides[LittleLong (brushes[i].firstside)];

	}

	free (brushes);
}

static void CM_Load_BrushSides (void)
{
	int i;
	dbrushside_t *sides;

	cm.num_brushsides = CM_ReadLump (BRUSH_SIDES, &sides, sizeof (dbrushside_t));
	cm.brushsides = malloc (cm.num_brushsides * sizeof (cbrushside_t));

	for (i = 0; i < cm.num_brushsides; i++)
	{
		cm.brushsides[i].plane = &cm.planes[LittleLong (sides[i].plane)];
		cm.brushsides[i].shaderref = &cm.shaderrefs [LittleLong (sides[i].shader)];
	}

	free (sides);
}

static void CM_Load_Vertices (aboolean load_rdata)
{
	int i;

	if (load_rdata)
	{
		cm.num_vertices = CM_ReadLump (VERTS, &cm.vertices, sizeof (dvertex_t));

		for (i= 0; i < cm.num_vertices; i++)
		{
			cm.vertices[i].lm_st[0] = LittleFloat(cm.vertices[i].lm_st[0]);
			cm.vertices[i].lm_st[1] = LittleFloat(cm.vertices[i].lm_st[1]);

			cm.vertices[i].tex_st[0] = LittleFloat (cm.vertices[i].tex_st[0]);
			cm.vertices[i].tex_st[1] = LittleFloat (cm.vertices[i].tex_st[1]);

			cm.vertices[i].v_norm [0] = LittleFloat (cm.vertices[i].v_norm[0]);
			cm.vertices[i].v_norm [1] = LittleFloat (cm.vertices[i].v_norm[1]);
			cm.vertices[i].v_norm [2] = LittleFloat (cm.vertices[i].v_norm[2]);

			cm.vertices[i].v_point[0] = LittleFloat (cm.vertices[i].v_point[0]);
			cm.vertices[i].v_point[1] = LittleFloat (cm.vertices[i].v_point[1]);
			cm.vertices[i].v_point[2] = LittleFloat (cm.vertices[i].v_point[2]);

			// TODO ? Colour 
		}
	}
}

static void CM_Load_Elems (aboolean load_rdata)
{
	int i;

	if (load_rdata)
	{
		cm.num_elems = CM_ReadLump (ELEMS, &cm.elems, sizeof (int));

		for (i = 0; i < cm.num_elems; i++)
			cm.elems[i] = LittleLong (cm.elems[i]);
	}
}

static void CM_Load_Fog (aboolean load_rdata)
{
	int i;
	dfogzone_t *fogs;

	if (load_rdata)
	{
		cm.num_fog = CM_ReadLump (FOGZONES, &fogs, sizeof(dfogzone_t));
		cm.fog = malloc (cm.num_fog * sizeof (cfogzone_t));

		for (i = 0; i < cm.num_fog; i++)
		{
			cm.fog[i].shader = &r_shaders[R_LoadShader (fogs[i].shadername, SHADER_BSP)];
			cm.fog[i].visibleside = LittleLong (fogs[i].visibleside);
			cm.fog[i].brush = &cm.brushes[LittleLong (fogs[i].brushnum)];
		}

		free (fogs);
	}
}

static void CM_Load_Faces (aboolean load_rdata)
{
	int i;
	dface_t *faces;

	if (load_rdata)
	{
		cm.num_faces = CM_ReadLump (FACES, &faces, sizeof(dface_t));
		cm.faces = malloc (cm.num_faces * sizeof (cface_t));

		for (i = 0; i < cm.num_faces; i++ )
		{
			cm.faces[i].elems = &cm.elems[LittleLong (faces[i].firstelem)];
			cm.faces[i].numelems = LittleLong (faces[i].numelems);

			cm.faces[i].verts = &cm.vertices [LittleLong (faces[i].firstvert)];
			cm.faces[i].numverts = LittleLong (faces[i].numverts);

			cm.faces[i].facetype = LittleLong (faces[i].facetype);

			cm.faces[i].lightmapnum = LittleLong (faces[i].lm_texnum);

			cm.faces[i].mins[0] = LittleFloat (faces[i].mins[0]);
			cm.faces[i].mins[1] = LittleFloat (faces[i].mins[1]);
			cm.faces[i].mins[2] = LittleFloat (faces[i].mins[2]);

			cm.faces[i].maxs[0] = LittleFloat (faces[i].maxs[0]);
			cm.faces[i].maxs[1] = LittleFloat (faces[i].maxs[1]);
			cm.faces[i].maxs[2] = LittleFloat (faces[i].maxs[2]);

			cm.faces[i].fognum = LittleLong (faces[i].fognum);

			cm.faces[i].lm_offset[0] = LittleLong (faces[i].lm_offset[0]);
			cm.faces[i].lm_offset[1] = LittleLong (faces[i].lm_offset[1]);

			cm.faces[i].lm_size[0] = LittleLong (faces[i].lm_size[0]);
			cm.faces[i].lm_size[1] = LittleLong (faces[i].lm_size[1]);
	
			cm.faces[i].mesh_cp[0] = LittleLong (faces[i].mesh_cp[0]);
			cm.faces[i].mesh_cp[1] = LittleLong (faces[i].mesh_cp[1]);

			cm.faces[i].normal[0] = LittleFloat (faces[i].v_norm[0]);
			cm.faces[i].normal[1] = LittleFloat (faces[i].v_norm[1]);
			cm.faces[i].normal[2] = LittleFloat (faces[i].v_norm[2]);

			cm.faces[i].v_orig[0] = LittleFloat (faces[i].v_orig[0]);
			cm.faces[i].v_orig[1] = LittleFloat (faces[i].v_orig[1]);
			cm.faces[i].v_orig[2] = LittleFloat (faces[i].v_orig[2]);

			cm.faces[i].shadernum = LittleLong (faces[i].shader);
		}

		free (faces);
	}
}

static void CM_Load_Lightmaps (aboolean load_rdata)
{
	if (load_rdata)
		cm.lightmapdata_size = CM_ReadLump (LIGHTMAPS, &cm.lightmapdata, 1); 
}

aboolean CM_LoadMap (const char *mapname, aboolean load_rdata)
{
	char buf[MAX_APATH];
	char fname[MAX_APATH];
	int file, filelen;
	int i;
	
	if (!strcmp (cm.name, mapname))
	{
		if (load_rdata == cm.r_data_loaded)
			return atrue;
		else 
			CM_FreeMap ();
	}

	if (cm.name[0])
		CM_FreeMap ();

	COM_StripExtension(mapname, buf);

	sprintf (fname, "maps/%s.bsp", buf);

	// read the file
	filelen = FS_OpenFile (fname, &file, FS_READ);

	if (!file || !filelen )
		return afalse;

	bspdata = malloc (filelen);
	FS_Read(bspdata, filelen, file);

	FS_FCloseFile (file);

	header = (bsp_header_t *)bspdata; 

	// swap the header
	header->id = LittleLong (header->id);
	header->ver = LittleLong (header->ver);

	for (i = 0; i < NUM_LUMPS; i++)
	{
		header->lumps[i].filelen = LittleLong (header->lumps[i].filelen);
		header->lumps[i].fileofs = LittleLong (header->lumps[i].fileofs);
	}
	
	// check id and version
	if (header->ver != BSPHEADER_VERSION)
	{	
		Con_Printf ("%s has wrong version number: %i should be %i\n", fname, header->ver, BSPHEADER_VERSION);
		return afalse;
	}

	if (header->id != BSPHEADER_ID)
	{
		Con_Printf ("%s is not a .bsp file!\n", fname);
		return afalse;
	}

	// now load the lumps
	CM_ReadLump (ENTITIES, &cm.entityspawn, 1);
	CM_Load_Shaderrefs (load_rdata);
	CM_Load_Planes ();
	CM_Load_Nodes ();
	CM_Load_LFaces (load_rdata);
	CM_Load_LBrushes ();
	CM_Load_BrushSides();
	CM_Load_Brushes ();
	CM_Load_Leaves ();
	CM_Load_Elems (load_rdata);
	CM_Load_Vertices (load_rdata);
	CM_Load_Lightmaps (load_rdata);
	CM_Load_Faces (load_rdata);
	CM_Load_Models (load_rdata);
	CM_Load_Fog (load_rdata);

	cm.r_data_loaded = load_rdata;

	A_strncpyz (cm.name, mapname, MAX_APATH);

	return atrue;
}

void CM_FreeMap (void)
{
	if (!cm.name[0])
		return;

	free (cm.brushes);
	free (cm.brushsides);
	free (cm.entityspawn);
	free (cm.lbrushes);
	free (cm.leaves);
	free (cm.models);
	free (cm.nodes);
	free (cm.shaderrefs);
	free (cm.vis);
	free (cm.planes);

	if (cm.r_data_loaded)
	{
		free (cm.faces);
		free (cm.fog);
		free (cm.lfaces);
		free (cm.lightmapdata);
		free (cm.vertices);
	}

	memset (&cm, 0, sizeof (cmap_t));
}
