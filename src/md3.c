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
#include "util.h"
#include "pak.h"
#include "cmap.h"
#include "md3.h"
#include "shader.h"
#include "console.h"
#include "io.h"

md3model2_t r_md3models[MAX_MD3_MODELS];
skin_t md3skins[MAX_MD3_SKINS];
int r_md3Skincount = 1;
int r_md3Modelcount = 1;

static aboolean R_LoadSkin(skin_t *skin, const char *name);
void MD3_Free (int num);

void MD3_Init (void)
{
	memset (r_md3models, 0, MAX_MD3_MODELS * sizeof(md3model2_t));
	r_md3Modelcount = 1;
	r_md3Skincount = 1;
}

void MD3_Shutdown (void)
{
/*	int i;

	for (i = 1; i < r_md3Modelcount; i++) MD3_Free(i);
*/
	r_md3Modelcount = 1;
	r_md3Skincount = 1;
}

/*
=====================
MD3_Free

=====================
*/
void MD3_Free (int num)
{
	md3model2_t *mod;
	md3mesh_t *mesh;
	int i;

	if (num <= 0 || num >= MAX_MD3_MODELS)
		return;

	mod = &r_md3models[num];

	if (mod->numframes)
		free (mod->frames);

	if ((mod->numframes > 0) && (mod->numtags > 0))
	{
		for (i = 0; i < mod->numframes; i++)
			free (mod->tags[i]);

		free (mod->tags);
	}

	if (mod->numskins > 0)
		free (mod->ids);

	if (mod->nummeshes > 0)
	{
		for (i = 0; i < mod->nummeshes; i++)
		{
			mesh = &mod->meshes[i];

			if (mesh->numelems > 0)
				free (mesh->elems);

			if (mesh->numtris > 0)
				free(mesh->tex_st);
	
			if (mesh->numverts > 0)
			{
				for (i = 0; i < mesh->numframes; i++)
					free (mesh->points[i]);

				free (mesh->points);
			}

			if (mesh->numskins > 0)
				free (mesh->skins);

			free (mesh);
		}
	}
}

/*
=====================
MD3_Load

This function is from GOLK. Slightly modified
=====================
*/
aboolean R_LoadMD3(md3model2_t *md3, const char *filename)
{
	byte				*fdata;
	byte				*data;
	byte				*mdata;
	md3header_t			*header;
	md3mesh_file_t		*mesh_header;
	int					i;
	int					j;
	int					k;
	char				texName[MAX_APATH];
	int					num;
	int					num2;
	int					num3;
	unsigned int		*pi;
	vec2_t				*pst;
	vec3_t				*pf;
	vec2_t				*pnorms;
	vec3_t				**ppf;
	vec2_t				**ppnorms;
	vec3_t				**ppnormals;
	md3mesh_t			*mesh;
	int					file;
	int					size;

	if (strlen(filename) > MAX_APATH) {
		Con_Printf("Model name exceeds %i\n", MAX_APATH);
		return afalse;
	}

	if (r_md3Modelcount >= MAX_MD3_MODELS)
	{
//		Error ("R_LoadMD3: too many models\n");
		return afalse;
	}

	size = FS_OpenFile(filename, &file, FS_READ);

	if (!size || !file)
		return afalse;

	data = fdata = malloc(size);
	FS_Read(fdata, size, file);
	FS_FCloseFile(file); 

	header = (md3header_t *)data;
	header->version = LittleLong(header->version);

	if (header->id != MD3_ID_HEADER)
	{
		Con_Printf("R_LoadMD3: %s is not a .md3\n", filename);
		free (fdata);
		return afalse;
	}

	if (header->version != MD3_ALIAS_VERSION) {
		Con_Printf("R_LoadMD3: %s has wrong version (%i should be %i)\n", header->version, MD3_ALIAS_VERSION);
		free(fdata);
		return afalse;
	}

	header->numboneframes = LittleLong(header->numboneframes);
	header->numtags = LittleLong(header->numtags);
	header->nummeshes = LittleLong(header->nummeshes);
	header->numskins = LittleLong(header->numskins);
	header->bone_offs = LittleLong(header->bone_offs);
	header->tag_offs = LittleLong(header->tag_offs);
	header->mesh_offs = LittleLong(header->mesh_offs);
	header->filesize = LittleLong(header->filesize);

	md3->numframes = header->numboneframes;
	md3->numtags =  header->numtags;
	md3->nummeshes = header->nummeshes;

	// frames
	if (md3->numframes && (header->filesize > header->bone_offs)) {
		md3->frames = (md3boneframe_t *)malloc(md3->numframes * sizeof(md3boneframe_t));

		data = fdata + header->bone_offs;

		num = md3->numframes;

		for (i = 0; i < num; i++) {

			// mins
			md3->frames[i].mins[0] = LittleFloat(*(float *) data);
			data += sizeof(float);
			md3->frames[i].mins[1] = LittleFloat (*(float *) data);
			data += sizeof(float);
			md3->frames[i].mins[2] = LittleFloat (*(float *) data);
			data += sizeof(float);

			// maxs
			md3->frames[i].maxs[3] = LittleFloat(*(float *) data);
			data += sizeof(float);
			md3->frames[i].maxs[4] = LittleFloat(*(float *) data);
			data += sizeof(float);
			md3->frames[i].maxs[5] = LittleFloat(*(float *) data);
			data += sizeof(float);

			// position
			md3->frames[i].pos[0] = LittleFloat(*(float *) data);
			data += sizeof(float);
			md3->frames[i].pos[1] = LittleFloat(*(float *) data);
			data += sizeof(float);
			md3->frames[i].pos[2] = LittleFloat(*(float *) data);
			data += sizeof(float);

			// scale
			md3->frames[i].scale = LittleFloat(*(float *) data);
			data += sizeof(float);

			// creator
			memcpy(md3->frames[i].creator, data, 16);
			data += 16;
		}
	} else {
		Con_Printf("R_LoadMD3: %s has no frames\n", filename);
		free(fdata);
		return afalse;
	}

	// tags
	if (md3->numtags && (header->filesize > header->tag_offs)) {
		data = fdata + header->tag_offs;

		md3->tags = (md3tag_t **) malloc(md3->numframes * sizeof(md3tag_t *));

		num = md3->numframes;
		for (i = 0; i < num; i++) {
			md3->tags[i] = (md3tag_t *) malloc(md3->numtags * sizeof(md3tag_t));

			num2 = md3->numtags;
			for (j = 0; j < num2; j++) {
				// name
				memcpy(md3->tags[i][j].name, data, MAX_APATH);
				data += MAX_APATH;

				// position
				md3->tags[i][j].pos[0] = LittleFloat(*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].pos[1] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].pos[2] = LittleFloat(*(float *) data);
				data += sizeof(float);

				// rotation
				md3->tags[i][j].rot[0][0] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[0][1] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[0][2] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[1][0] = LittleFloat(*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[1][1] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[1][2] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[2][0] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[2][1] = LittleFloat (*(float *) data);
				data += sizeof(float);
				md3->tags[i][j].rot[2][2] = LittleFloat (*(float *) data);
				data += sizeof(float);
			}
		}
	} else {
		md3->tags = NULL;
	}

	// meshes
	if (md3->nummeshes && (header->filesize > header->mesh_offs)) {
		md3->meshes = (md3mesh_t *) malloc(md3->nummeshes * sizeof(md3mesh_t));

		mdata = fdata + header->mesh_offs;

		num = md3->nummeshes;

		for (i = 0; i < num; i++) {
			mesh = &md3->meshes[i];

			// mesh header
			mesh_header = (md3mesh_file_t *) data;
			mesh_header->flags = LittleLong(mesh_header->flags);
			mesh_header->numframes = LittleLong(mesh_header->numframes);
			mesh_header->numskins = LittleLong(mesh_header->numskins);
			mesh_header->numverts = LittleLong(mesh_header->numverts);
			mesh_header->skin_offs = LittleLong(mesh_header->skin_offs);
			mesh_header->tc_offs = LittleLong(mesh_header->tc_offs);
			mesh_header->vert_offs = LittleLong(mesh_header->vert_offs);
			mesh_header->meshsize = LittleLong(mesh_header->meshsize);

			if (mesh_header->id == MD3_ID_HEADER) {
				A_strncpyz(mesh->name, mesh_header->name, MAX_APATH);

				mesh->numframes = mesh_header->numframes;
				mesh->numskins = mesh_header->numskins;
				mesh->numverts = mesh_header->numverts;
				mesh->numtris = mesh_header->numtris;
				mesh->flags = mesh_header->flags;

				// skins
				if (mesh->numskins && (mesh_header->meshsize > mesh_header->skin_offs)) {
					mesh->skins = (int *)malloc(mesh->numskins * sizeof(int));

					data = mdata + mesh_header->skin_offs;

					num2 = mesh->numskins;

					for (j = 0; j < num2; j++) {
						COM_StripExtension(data, texName);
						data += MAX_APATH;
						mesh->skins[j] = R_LoadShader(texName, SHADER_MD3);
					}
				} else {
					mesh->skins = NULL;
				}

				// indizes
				if (mesh->numtris && (mesh_header->meshsize > mesh_header->elem_offs)) {
					mesh->elems = (unsigned int *) malloc(mesh->numtris * 3 * sizeof(unsigned int));
					mesh->numelems = mesh->numtris*3;
					data = mdata + mesh_header->elem_offs;

					num2 = mesh->numtris * 3;
					pi = mesh->elems;

					for (j = 0; j < num2; j++, pi++) {
						*pi = LittleLong(*((long *)data));
						data += sizeof(long);
					}
				} else {
					mesh->elems = NULL;
				}

				// texcoords
				if (mesh->numverts && (mesh_header->meshsize > mesh_header->tc_offs)) {
					data = mdata + mesh_header->tc_offs;

					mesh->tex_st = (vec2_t *)malloc(mesh->numverts * sizeof(vec2_t));

					num2 = mesh->numverts;
					pst = mesh->tex_st;

					for (j = 0; j < num2; j++, pst++) {
						(*pst)[0] = LittleFloat(*((float *)data));
						data += sizeof(float);
						(*pst)[1] = LittleFloat (*((float *)data));
						data += sizeof(float);
					}
				} else {
					mesh->tex_st = NULL;
				}

				// vertices
				if (mesh->numverts && (mesh_header->meshsize > mesh_header->vert_offs)) {
					mesh->points = (vec3_t **)malloc(mesh->numframes * sizeof(vec3_t *));
					mesh->norms = (vec2_t **)malloc(mesh->numframes * sizeof(vec2_t *));

					data = mdata + mesh_header->vert_offs;

					num3 = mesh->numframes;
					ppf = mesh->points;
					ppnorms = mesh->norms;
			
					for (j = 0; j < num3; j++, ppf++, ppnorms++, ppnormals++) {
						pf = *ppf = (vec3_t *)malloc(num2 * sizeof(vec3_t));
						pnorms = *ppnorms = (vec2_t *)malloc(num2 * sizeof(vec2_t));
				
						for (k = 0; k < num2; k++, pf++, pnorms++) {
							(*pf)[0] = (float)(*((short *)data)) * MD3_XYZ_SCALE;
							data += sizeof(short);
							(*pf)[1] = (float)(*((short *)data)) * MD3_XYZ_SCALE;
							data += sizeof(short);
							(*pf)[2] = (float)(*((short *)data)) * MD3_XYZ_SCALE;
							data += sizeof(short);

							(*pnorms)[0] = (LittleFloat((float)(*data++))) / 256.f;
							(*pnorms)[1] = (LittleFloat((float)(*data++))) / 256.f;
						}
					}

					mdata += mesh_header->meshsize;
				} else {
					mesh->points = NULL;
					mesh->norms = NULL;
				}
			} else {
				free(fdata);
				return afalse;
			}
		}
	} else {
		md3->meshes = NULL;
	}

	A_strncpyz(md3->name, filename, MAX_OSPATH);

	free(fdata);

	return atrue;
}

int R_RegisterModel (const char *name)
{
	int i;

	// Check if already existing 
	for (i = 1; i < r_md3Modelcount; i++)
	{
		if (!strcmp(r_md3models[i].name, name))
			return i;
	}

	if (R_LoadMD3(&r_md3models[r_md3Modelcount], name))
		return r_md3Modelcount++;

	return 0;
}

int R_RegisterSkin (const char *name)
{
	int i;

	// Check if already existing 
	for (i = 1; i < r_md3Skincount; i++)
	{
		if (!strcmp(name, md3skins[i].name))
			return i;
	}

	if (R_LoadSkin (&md3skins[r_md3Skincount], name))
		return r_md3Skincount++;
	
	return 0;
}

static aboolean R_LoadSkin(skin_t *skin, const char *name) 
{
	int file, len, meshcount = 0;
	char *token, *buf, *tmp, *comma, shadername[MAX_APATH];

	if (r_md3Skincount >= MAX_MD3_SKINS)
	{
//		Error ("R_LoadSkin: too many skins\n");
		return afalse;
	}

	len = FS_OpenFile (name, &file, FS_READ);

	if (!len || !file) 
		return afalse;

	buf = malloc (len + 1);
	*(buf + len) = 0;

	FS_Read (buf, len, file);
	FS_FCloseFile(file); 

	strcpy(skin->name, name);
	tmp = buf;
	
	COM_BeginParseSession();

	while (tmp) 
	{
		token = COM_Parse (&tmp);

		if (!token[0])
			continue;

		comma = strchr(token, ',');

		if (A_strncmp(token, "tag_", 4))
		{
			*comma++ = 0;
			COM_StripExtension (comma, shadername);

			if (meshcount >= MAX_MD3_MESHES) 
			{
				Con_Printf("R_LoadSkin: Too many meshes!\n");
				return afalse;
			}

			strcpy (skin->skins[meshcount].mesh_name, token);
			skin->skins[meshcount].shaderref = R_LoadShader (shadername, SHADER_MD3);
			meshcount++;
		}
		else 
		{
			// ????

		}
	}

	free (buf);
	skin->num_mesh_skins = meshcount;

	return atrue;
}

