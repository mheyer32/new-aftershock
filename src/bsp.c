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
#include "util.h"
#include "bsp.h"
#include "entity.h"
#include "a_shared.h"
#include "console.h"
#include "io.h"
#include "opengl.h"
#include "render.h"
#include "shader.h"

#define BSPHEADERID  (*(int*)"IBSP")
#define BSPVERSION 46



/* BSP lumps in the order they appear in the header */
enum 
{
    ENTITIES,
    SHADERREFS,
    DPLANES,
    NODES,
    LEAFS,
    LFACES,
    BRUSH_LIST,
    MODELS,
    BRUSHES,
    BRUSH_SIDES,
    VERTS,
    ELEMS,
    FOGZONES,
    FACES,
    LIGHTMAPS,
    LIGHTGRID,
    VISIBILITY,
    NUM_LUMPS
};


	
static struct header
{
    int id, ver;
    struct { int fileofs, filelen; } lump[NUM_LUMPS];
} *bspheader;

static byte *bspdata;
static int bsplen =0;
static int bsploaded =0;
static int load_r_data = 1;

static int readlump(int lump, void** mem, size_t elem);

#define READLUMP(lump,val) \
      r_num##val = readlump(lump, (void**)&r_##val, sizeof(*(r_##val)))



map_t map ;



static void Check_Lump (int num )
{

	if (bspheader->lump[num].fileofs + bspheader->lump[num].filelen > bsplen )
		Error ( "Bad Lump Data ! " );
}



static void Load_Shaderrefs ( dshaderref_t * in, cshaderref_t * out ,int num )
{

	int i;
	map.shadernums = malloc (num * sizeof (int));
	for (i=0;i<num;i++,in++,out++ )
	{
		out->contents=LittleLong(in->contents);
		out->flags=LittleLong (in->flags);

		out->shader=R_RegisterShader(in->name);
		map.shadernums[i] = out->shader;

	}
}
static void Load_Planes ( dplane_t *in , cplane_t *out ,int num )
{

	int i,j;
	for (i=0;i<num;i++,in++,out++ )
	{
		out->dist= LittleFloat (in->offset);
		out->normal[0] = LittleFloat (in->vec[0]);
		out->normal[1] = LittleFloat (in->vec[1]);
		out->normal[2] = LittleFloat (in->vec[2]);

		out->type=4;
		for (j=0;j<3;j++)
		{
			if (out->normal[j]==1.0)
				out->type=j;
		}
		SetPlaneSignbits (out); 

	}

}


static void Load_Nodes ( dnode_t *in, cnode_t *out ,int num )
{
	int i;

	for (i=0;i<num;i++,in++,out++)
	{
		out->mins[0]= (float ) LittleLong (in->mins[0]);
		out->mins[1]= (float ) LittleLong (in->mins[1]);
		out->mins[2]= (float ) LittleLong (in->mins[2]);

		out->maxs[0]= (float ) LittleLong (in->maxs[0]);
		out->maxs[1]= (float ) LittleLong (in->maxs[1]);
		out->maxs[2]= (float ) LittleLong (in->maxs[2]);

		out->children[0] = LittleLong (in->children[0]);
		out->children[1] = LittleLong (in->children[1]);

		out->plane = &map.planes[LittleLong(in->plane)];

	}


}

static void Load_Leafs ( dleaf_t *in , cleaf_t * out,int num )
{

	int i;

	for (i=0;i<num;i++,in++,out++)
	{
		out->mins[0]=(float)LittleLong(in->mins[0]);
		out->mins[1]=(float)LittleLong(in->mins[1]);
		out->mins[2]=(float)LittleLong(in->mins[2]);
		
		out->maxs[0]=(float)LittleLong(in->maxs[0]);
		out->maxs[1]=(float)LittleLong(in->maxs[1]);
		out->maxs[2]=(float)LittleLong(in->maxs[2]);

		out->area= LittleLong (in->area);
		out->cluster =LittleLong (in->cluster);
		out->firstbrush =LittleLong (in->firstbrush);
		out->numbrushes = LittleLong (in->numbrushes);

		out->firstface = LittleLong (in->firstface);
		out->numfaces = LittleLong (in->numfaces);
	}


}

static void Load_LFaces (int * lfaces,int num)
{
	int i;

	for (i=0;i<num;i++,lfaces++)
	{
		lfaces[0]=LittleLong (lfaces[0]);
	}

}

static void Load_LBrushes (int * lbrushes ,int num)
{
	int i;

	for (i=0;i<num;i++,lbrushes++)
	{
		lbrushes[0]=LittleLong (lbrushes[0]);

	}
}

static void Load_Models (dmodel_t *in,cmodel_t * out ,int num )
{
	int i;

	for (i=0;i<num;i++,in++,out++)
	{
		out->mins[0] = (float ) LittleLong (in->mins[0]);
		out->mins[1] = (float ) LittleLong (in->mins[1]);
		out->mins[2] = (float ) LittleLong (in->mins[2]);

		out->maxs[0] = (float ) LittleLong (in->maxs[0]);
		out->maxs[1] = (float ) LittleLong (in->maxs[1]);
		out->maxs[2] = (float ) LittleLong (in->maxs[2]);

		out->brushes = & map.brushes [LittleLong (in->firstbrush)];
		out->numbrushes = LittleLong (in->numbrushes);

		out->firstface = LittleLong (in->firstface);
		out->numfaces  = LittleLong (in->numfaces);
	}

}

static void Load_Brushsides ( dbrushside_t * in ,cbrushside_t * out ,int num )
{
	int i;

	for (i=0;i<num ; i++,in++,out++)
	{
		out->plane = &map.planes[LittleLong (in->plane)];
		out->shader = &map.shaderrefs[LittleLong (in->shader)];
	}

}

static void Load_Brushes ( dbrush_t * in , cbrush_t  * out , int num )
{
	int i;

	for (i=0;i<num ; i++,in++,out++ )
	{
		out->numsides = LittleLong ( in->numsides );

		out->shader = &map.shaderrefs [LittleLong ( in->shader)];

		out->sides= &map.brushsides [ LittleLong (in->firstside)];
	}

}

static void Load_Vertices ( vertex_t * v, int num )
{
	int i;

	for (i=0;i<num ;i++ ,v++ )
	{
		v->lm_st[0] = LittleFloat ( v->lm_st[0]);
		v->lm_st[1] = LittleFloat ( v->lm_st[1]);

		v->tex_st[0] = LittleFloat (v->tex_st[0]);
		v->tex_st[1] = LittleFloat (v->tex_st[1]);

		v->v_norm[0] = LittleFloat (v->v_norm[0]);
		v->v_norm[1] = LittleFloat (v->v_norm[1]);
		v->v_norm[2] = LittleFloat (v->v_norm[2]);

		v->v_point[0] = LittleFloat (v->v_point[0]);
		v->v_point[1] = LittleFloat (v->v_point[1]);
		v->v_point[2] = LittleFloat (v->v_point[2]);

	}

}

static void Load_Elems ( int * elems ,int num )
{
	int i;

	for (i=0;i<num;i++,elems++)
	{
		elems[0] = LittleLong ( elems[0]);

	}

}

static void Load_Faces (dface_t *in, cface_t *out ,int num )
{
	int i;

	for (i=0;i<num ;i++,in++,out++)
	{
		out->elems = &map.elems[LittleLong (in->firstelem)];
		out->numelems = LittleLong (in->numelems);

		out->verts =  & map.vertices [LittleLong (in->firstvert)];
		out->numverts = LittleLong (in->numverts);

		out->facetype = LittleLong (in->facetype);

		out->shadernum = LittleLong (in->shader);

		out->lm_texnum = LittleLong (in->lm_texnum);


		out->lm_offset[0] = LittleLong (in->lm_offset[0]);
		out->lm_offset[1] = LittleLong (in->lm_offset[1]);
	
		out->lm_size[0] = LittleLong (in->lm_size[0]);
		out->lm_size[1] = LittleLong (in->lm_size[1]);

		out->mins[0] = LittleFloat (in->mins[0]);
		out->mins[1] = LittleFloat (in->mins[1]);
		out->mins[2] = LittleFloat (in->mins[2]);

		out->maxs[0] = LittleFloat (in->maxs[0]);
		out->maxs[1] = LittleFloat (in->maxs[1]);
		out->maxs[2] = LittleFloat (in->maxs[2]);

		out->mesh_cp[0] = LittleLong (in->mesh_cp[0]);
		out->mesh_cp[1] = LittleLong (in->mesh_cp[1]);
	
		out->unknown = LittleLong (in->unknown);

		out->v_norm[0] = LittleFloat (in->v_norm[0]);
		out->v_norm[1] = LittleFloat (in->v_norm[1]);
		out->v_norm[2] = LittleFloat (in->v_norm[2]);

		out->v_orig[0] = LittleFloat (in->v_orig[0]);
		out->v_orig[1] = LittleFloat (in->v_orig[1]);
		out->v_orig[2] = LittleFloat (in->v_orig[2]);

	}


}

// GL Code : FIXME ??

static void Load_Lightmaps (byte * data ,int num )
{
	 int i, texsize = (128*128*3);
    
    map.lightmaps = malloc (num * sizeof (int ));
	glGenTextures (num , map.lightmaps );
    
    for (i=0; i < num; ++i)
    {
	if (r_gamma->value > 1.0)
	{
	    int j, val;
	    byte *c;

	    c = &data[i * texsize];
	    for (j=0; j < texsize; j++, c++)
	    {
		val = *c * r_gamma->value;
		if (val > 255) val = 255;
		*c = val;
	    }
	}
	
	GL_BindTexture(GL_TEXTURE_2D, map.lightmaps[i]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB,
		     GL_UNSIGNED_BYTE, &data[i * texsize]);
    }

}

static void Load_Vis (visibility_t * vis,int num )
{

		vis->numclusters=LittleLong (vis->numclusters);
		vis->rowsize=LittleLong (vis->rowsize);
}


int BSP_Load_Map(char *mapname )
{

	int file ,len,i;
	char filename [MAX_APATH];
	dshaderref_t * dshaders ;
	dplane_t * dplanes =NULL;
	dnode_t * dnodes =NULL;
	dleaf_t * dleafs =NULL;
	dmodel_t * dmodels =NULL;
	dbrush_t * dbrushes =NULL;
	dbrushside_t * dbrushsides=NULL ;
	dface_t * dfaces =NULL;
	
	byte * lightmaps = NULL;


	if (bsploaded)
		Error ("BSP_LoadMap : bsploaded ");


	//COM_StripExtension(mapname,filename);
	sprintf (filename,"maps/%s.bsp",mapname);

	len =FS_OpenFile (filename,& file ,FS_READ );

	

	if (!len || ! file )
	{
		return 0;
	}

	bsplen = len;
	bspdata =malloc (len);
	
	FS_Read (bspdata ,len, file );

	FS_FCloseFile (file );


	bspheader = (struct header*)bspdata;

    bspheader->ver= LittleLong (bspheader->ver);
    for (i=0; i<NUM_LUMPS; i++) {
     bspheader->lump[i].fileofs= LittleLong(bspheader->lump[i].fileofs);
     bspheader->lump[i].filelen= LittleLong(bspheader->lump[i].filelen);
    }
    

	// ENTITIES :
	Check_Lump (ENTITIES);
	Start_Entity_Parsing(bspdata+ bspheader->lump[ENTITIES].fileofs,bspheader->lump[ENTITIES].filelen);


	// SHADERREFS :

	Check_Lump (SHADERREFS);
	dshaders=malloc (bspheader->lump[SHADERREFS].filelen);
	
	memcpy (dshaders,bspdata+bspheader->lump[SHADERREFS].fileofs,bspheader->lump[SHADERREFS].filelen);
	
	map.num_shaders=bspheader->lump[SHADERREFS].filelen/ sizeof (dshaderref_t );
	map.shaderrefs=malloc (map.num_shaders* sizeof (cshaderref_t));

	Load_Shaderrefs(dshaders,map.shaderrefs,map.num_shaders);
	free (dshaders);

	// PLANES :
	Check_Lump(DPLANES);

	dplanes =malloc (bspheader->lump[DPLANES].filelen);
	memcpy (dplanes,bspdata+bspheader->lump[DPLANES].fileofs,bspheader->lump[DPLANES].filelen);

	map.num_planes=bspheader->lump[DPLANES].filelen /sizeof (dplane_t);
	map.planes=malloc (map.num_planes*sizeof (cplane_t ));

	Load_Planes (dplanes,map.planes,map.num_planes);

	free (dplanes);

	// NODES :
	Check_Lump(NODES);

	dnodes = malloc (bspheader->lump[NODES].filelen);
	memcpy (dnodes,bspdata+bspheader->lump[NODES].fileofs,bspheader->lump[NODES].filelen);

	map.num_nodes=bspheader->lump[NODES].filelen / sizeof (dnode_t);
	map.nodes= malloc ( map.num_nodes * sizeof ( cnode_t ));

	Load_Nodes (dnodes,map.nodes,map.num_nodes);
	
	free (dnodes);


	// LEAFS :
	Check_Lump (LEAFS);

	dleafs = malloc (bspheader->lump[LEAFS].filelen);
	memcpy  (dleafs,bspdata+bspheader->lump[LEAFS].fileofs,bspheader->lump[LEAFS].filelen);

	map.num_leafs = bspheader->lump[LEAFS].filelen /sizeof (dleaf_t );
	map.leafs = malloc (map.num_leafs * sizeof (cleaf_t ));

	Load_Leafs (dleafs,map.leafs,map.num_leafs);

	free (dleafs );

	// LFACES :
	if (load_r_data )
	{
		Check_Lump(LFACES);

		map.lfaces =malloc ( bspheader->lump[LFACES].filelen);
		map.num_lfaces = bspheader->lump[LFACES].filelen / sizeof (int);

		memcpy (map.lfaces,bspdata+bspheader->lump[LFACES].fileofs,bspheader->lump[LFACES].filelen);

		Load_LFaces (map.lfaces,map.num_lfaces);

	}
	// BRUSH_LIST :
	Check_Lump(BRUSH_LIST);

	map.lbrushes= malloc (bspheader->lump[BRUSH_LIST].filelen);
	map.num_lbrushes= bspheader->lump[BRUSH_LIST].filelen /sizeof (int );

	memcpy (map.lbrushes,bspdata+ bspheader->lump[BRUSH_LIST].fileofs,bspheader->lump[BRUSH_LIST].filelen);

	Load_LBrushes (map.lbrushes,map.num_lbrushes);

	// BRUSH_SIDES :
	Check_Lump(BRUSH_SIDES);

	dbrushsides = malloc ( bspheader->lump[BRUSH_SIDES].filelen);
	memcpy (dbrushsides, bspdata + bspheader->lump[BRUSH_SIDES].fileofs, bspheader->lump[BRUSH_SIDES].filelen);

	map.num_brushsides= bspheader->lump[BRUSH_SIDES].filelen / sizeof (dbrushside_t );
	map.brushsides = malloc ( map.num_brushsides * sizeof (cbrushside_t ));

	Load_Brushsides (dbrushsides ,map.brushsides,map.num_brushsides);

	free ( dbrushsides );

	// BRUSHES :
	Check_Lump ( BRUSHES);

	dbrushes = malloc ( bspheader->lump[BRUSHES].filelen);
	memcpy (dbrushes , bspdata + bspheader ->lump[BRUSHES].fileofs ,bspheader->lump[BRUSHES].filelen);

	map.num_brushes =  bspheader->lump[BRUSHES].filelen / sizeof (dbrush_t );
	map.brushes = malloc (map.num_brushsides * sizeof ( cbrush_t));

	Load_Brushes ( dbrushes , map.brushes, map.num_brushes);

	free ( dbrushes );

	// MODELS :
	Check_Lump(MODELS);

	dmodels = malloc ( bspheader->lump[MODELS].filelen);
	memcpy (dmodels,bspdata +bspheader->lump[MODELS].fileofs,bspheader->lump[MODELS].filelen);

	map.num_models= bspheader->lump[MODELS].filelen / sizeof (dmodel_t );
	map.models =malloc ( map.num_models * sizeof (cmodel_t ));

	Load_Models (dmodels,map.models,map.num_models);




	// VERTS :
	Check_Lump (VERTS);

	map.vertices = malloc ( bspheader->lump[VERTS].filelen);
	map .num_vertices = bspheader->lump[VERTS].filelen / sizeof (vertex_t );

	memcpy ( map.vertices , bspdata+ bspheader->lump[VERTS].fileofs, bspheader->lump[VERTS].filelen);

	Load_Vertices ( map.vertices, map.num_vertices );

	// ELEMS :
	if (load_r_data )
	{
	Check_Lump (ELEMS);

	map.elems= malloc (bspheader->lump[ELEMS].filelen);
	map.num_elems = bspheader->lump[ELEMS].filelen / sizeof (int );

	memcpy ( map.elems, bspdata + bspheader->lump[ELEMS].fileofs , bspheader->lump[ELEMS].filelen);
	
	Load_Elems ( map.elems,map.num_elems);

	}
// TODO : FOG !
// -------------------------------------

	// FACES :
	if (load_r_data )
	{
		Check_Lump(FACES);

		dfaces = malloc ( bspheader->lump[FACES].filelen);
		memcpy ( dfaces, bspdata + bspheader->lump[FACES].fileofs ,bspheader->lump[FACES].filelen );

		map.num_faces = bspheader->lump [FACES].filelen / sizeof (dface_t ) ;
		map .faces = malloc ( map.num_faces * sizeof ( cface_t ));

		Load_Faces (dfaces,map.faces,map.num_faces);

		free ( dfaces);
	}


	// LIGHTMAPS :

	if (load_r_data)
	{
		Check_Lump(LIGHTMAPS);

		lightmaps = malloc ( bspheader->lump[LIGHTMAPS].filelen);
		memcpy ( lightmaps , bspdata + bspheader->lump[LIGHTMAPS].fileofs ,bspheader->lump[LIGHTMAPS].filelen);

		map.num_lightmap = bspheader->lump[LIGHTMAPS].filelen / ( 128 * 128 * 3 );

		Load_Lightmaps (lightmaps,map.num_lightmap);

		free ( lightmaps);
	}

// TODO : LIGHTGRID !!!


	// VISIBILITY

	Check_Lump (VISIBILITY); 

	map.vis = malloc ( bspheader->lump[VISIBILITY].filelen);
	memcpy (map.vis,bspdata+ bspheader->lump[VISIBILITY].fileofs, bspheader->lump[VISIBILITY].filelen);

	map.num_vis = bspheader->lump [VISIBILITY].filelen / sizeof (visibility_t);
	
	Load_Vis (map.vis,map.num_vis);




	// DONE !
	A_strncpyz (map.name,mapname ,MAX_APATH);


	if (load_r_data)
		map.r_data_loaded=1;
	else
		map.r_data_loaded=0;

	bsploaded =1;




	return 1;



}




void BSP_free_Map (void )
{
	// TODO 

	if (!bsploaded)
	{

		Error ("BSP_free_Map  : no map loaded !");
	}

	free (map.brushes);
	free (map.brushsides);
	free (map.lbrushes);
	free (map.leafs);

	glDeleteTextures (map.num_lightmap,map.lightmaps);

	free (map.models);
	free (map.nodes);
	free (map.planes);
	free (map.shaderrefs);
	free (map.shadernums);
	free (map.vertices);
	free (map.vis);


	if (map.r_data_loaded)
	{
		free (map.faces);
		free (map.elems);
		free (map.lfaces);
		free (map.lightmaps);
	}

	memset ( &map,0,sizeof (map));

	bsploaded=0;




}











static int
readlump(int lump, void** mem, size_t elem)
{
    int len = bspheader->lump[lump].filelen;
    int num = len / elem;
    *mem = malloc(len);

    memcpy(*mem, bspdata + bspheader->lump[lump].fileofs, num * elem);


    {
	int i;
	for (i=0; i<(num*elem)>>2; i++)
	{
	    unsigned *ptr = (unsigned *)*mem;
	    ptr[i]=LittleLong(ptr[i]);
	}
    }
   

    return num;
}
