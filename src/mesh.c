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
#include "cmap.h"
#include "render.h"
#include "mesh.h"

#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)

static void Mesh_Create(cface_t *face, mesh_t *mesh);

void Mesh_CreateAll(void)
{
    int i;    
    
	// Count meshes
    for (i = 0, r_nummeshes = 0; i < cm.num_faces; i++)
		if (cm.faces[i].facetype != FACETYPE_MESH)
			r_nummeshes++;

    r_meshes = (mesh_t *)malloc(r_nummeshes * sizeof(mesh_t));
	r_facemeshes = (int *)malloc(cm.num_faces * sizeof(int));

    for (i = 0, r_nummeshes = 0; i < cm.num_faces; i++)
    {
		if (cm.faces[i].facetype == FACETYPE_MESH) {
			Mesh_Create(&cm.faces[i], &r_meshes[r_nummeshes]);
			r_facemeshes[i] = r_nummeshes++;
		}
    }
}

void Mesh_FreeAll(void)
{
    int i;

    for (i = 0; i < r_nummeshes; i++)
    {
		free(r_meshes[i].points);

		// tex_st and lm_st are part of points: don't free
		free(r_meshes[i].elems);
    }

	free(r_facemeshes);
    free(r_meshes);
}

static int Mesh_FindLevel(vec3_t *v)
{
    int level;
    vec3_t a, b, dist;

    // Subdivide on the left until tolerance is reached
    for (level = 0; level < r_maxmeshlevel-1; level++)
    {
		// Subdivide on the left
		VectorAvg(v[0], v[1], a);
		VectorAvg(v[1], v[2], b);
		VectorAvg(a, b, v[2]);

		// Find distance moved
		VectorSubtract(v[2], v[1], dist);

		// Check for tolerance
		if (DotProduct(dist, dist) < r_subdivisiontol * r_subdivisiontol)
			break;

		// Insert new middle vertex
		VectorCopy(a, v[1]);
    }

    return level;
}

static void Mesh_FindSize(int *numcp, vec3_t *cp, int *size)
{
    int u, v, found, level;
    float *a, *b;
    vec3_t test[3];
    
    // Find non-coincident pairs in u direction
    found = 0;
    for (v=0; v < numcp[1]; v++)
    {
		for (u=0; u < numcp[0]-1; u += 2)
		{
			a = cp[v * numcp[0] + u];
			b = cp[v * numcp[0] + u + 2];

			if (!VectorCompare(a,b))
			{
				found = 1;
				break;
			}
		}

		if (found)
			break;
    }

    if (!found) {
		Com_Error( ERR_FATAL, "Bad mesh control points");
		return;
	}

    // Find subdivision level in u
    VectorCopy(a, test[0]);
    VectorCopy((a+3), test[1]);
    VectorCopy(b, test[2]);
    level = Mesh_FindLevel(test);
    size[0] = (LEVEL_WIDTH(level) - 1) * ((numcp[0]-1) / 2) + 1;
    
    // Find non-coincident pairs in v direction
    found = 0;
    for (u=0; u < numcp[0]; u++)
    {
		for (v=0; v < numcp[1]-1; v += 2)
		{
			a = cp[v * numcp[0] + u];
			b = cp[(v + 2) * numcp[0] + u];
			if (!VectorCompare(a,b))
			{
				found = 1;
				break;
			}
		}

		if (found) 
			break;
    }

    if (!found) {
		Com_Error( ERR_FATAL, "Bad mesh control points");
		return;
	}

    // Find subdivision level in v
    VectorCopy(a, test[0]);
    VectorCopy((a+numcp[0]*3), test[1]);
    VectorCopy(b, test[2]);
    level = Mesh_FindLevel(test);
    size[1] = (LEVEL_WIDTH(level) - 1)* ((numcp[1]-1) / 2) + 1;    
}

static void Mesh_FillCurve3(int numcp, int size, int stride, vec3_t *p)
{
    int step, halfstep, i, mid;
    vec3_t a, b;

    step = (size-1) / (numcp-1);

    while (step > 0)
    {
		halfstep = step >> 1;

		for (i=0; i < size-1; i += step<<1)
		{
			mid = (i+step)*stride;
			VectorAvg(p[i*stride], p[mid], a);
			VectorAvg(p[mid], p[(i+step*2)*stride], b);
			VectorAvg(a, b, p[mid]);

			if (halfstep > 0)
			{
				VectorCopy(a, p[(i+halfstep)*stride]);
				VectorCopy(b, p[(i+3*halfstep)*stride]);
			}
		}
		
		step >>= 1;
    }
}

static void Mesh_FillCurve2(int numcp, int size, int stride, vec2_t *p)
{
    int step, halfstep, i, mid;
    vec2_t a, b;

    step = (size-1) / (numcp-1);

    while (step > 0)
    {
		halfstep = step >> 1;

		for (i=0; i < size-1; i += step<<1)
		{
			mid = (i+step)*stride;
			Vector2Avg(p[i*stride], p[mid], a);
			Vector2Avg(p[mid], p[(i+step*2)*stride], b);
			Vector2Avg(a, b, p[mid]);

			if (halfstep > 0)
			{
				Vector2Copy(a, p[(i+halfstep)*stride]);
				Vector2Copy(b, p[(i+3*halfstep)*stride]);
			}
		}
		
		step >>= 1;
    }
}

static void Mesh_FillCurveC(int numcp, int size, int stride, colour_t *p)
{
    int step, halfstep, i, mid;
    colour_t a, b;

    step = (size-1) / (numcp-1);

    while (step > 0)
    {
		halfstep = step >> 1;

		for (i=0; i < size-1; i += step<<1)
		{
			mid = (i+step)*stride;
			Vector4Avg(p[i*stride], p[mid], a);
			Vector4Avg(p[mid], p[(i+step*2)*stride], b);
			Vector4Avg(a, b, p[mid]);

			if (halfstep > 0)
			{
				Vector4Copy(a, p[(i+halfstep)*stride]);
				Vector4Copy(b, p[(i+3*halfstep)*stride]);
			}
		}
		
		step >>= 1;
    }
}

static void Mesh_FillPatch3(int *numcp, int *size, vec3_t *p)
{
    int step, u, v;

    // Fill in control points in v direction
    step = (size[0]-1) / (numcp[0]-1);
    for (u = 0; u < size[0]; u += step)
    {
		Mesh_FillCurve3(numcp[1], size[1], size[0], p + u);
    }

    // Fill in the rest in the u direction
    for (v = 0; v < size[1]; v++)
    {
		Mesh_FillCurve3(numcp[0], size[0], 1, p + v * size[0]);
    }
}

static void Mesh_FillPatch2(int *numcp, int *size, vec2_t *p)
{
    int step, u, v;

    // Fill in control points in v direction
    step = (size[0]-1) / (numcp[0]-1);
    for (u = 0; u < size[0]; u += step)
    {
		Mesh_FillCurve2(numcp[1], size[1], size[0], p + u);
    }

    // Fill in the rest in the u direction
    for (v = 0; v < size[1]; v++)
    {
		Mesh_FillCurve2(numcp[0], size[0], 1, p + v * size[0]);
    }
}

static void Mesh_FillPatchC(int *numcp, int *size, colour_t *p)
{
    int step, u, v;

    // Fill in control points in v direction
    step = (size[0]-1) / (numcp[0]-1);    
    for (u = 0; u < size[0]; u += step)
    {
		Mesh_FillCurveC(numcp[1], size[1], size[0], p + u);
    }

    // Fill in the rest in the u direction
    for (v = 0; v < size[1]; v++)
    {
		Mesh_FillCurveC(numcp[0], size[0], 1, p + v * size[0]);
    }
}

static void Mesh_Create(cface_t *face, mesh_t *mesh)
{
    int step[2], size[2], len, i, u, v, p;
    vec3_t *cp;
    cvertex_t *vert;

    cp = (vec3_t *)malloc(face->numverts * sizeof(vec3_t));
    vert = face->verts;

    for (i=0; i < face->numverts; i++)
    {
		VectorCopy(vert->v_point, cp[i]);
		vert++;
    }

    // Find the degree of subdivision in the u and v directions
    Mesh_FindSize(face->mesh_cp, cp, size);
    free(cp);

    // Allocate space for mesh
    len = size[0] * size[1];
    mesh->size[0] = size[0];
    mesh->size[1] = size[1];
	mesh->msize = len;
    mesh->points = (vec3_t *)malloc(len * (sizeof(vec3_t) +
					  2 * sizeof(vec2_t) +
					 sizeof(colour_t)));
    mesh->colour = (colour_t *)(mesh->points + len);
    mesh->tex_st = (vec2_t *)(mesh->colour + len);
    mesh->lm_st = mesh->tex_st + len;

    // Fill in sparse mesh control points
    step[0] = (size[0]-1) / (face->mesh_cp[0]-1);
    step[1] = (size[1]-1) / (face->mesh_cp[1]-1);
    vert = face->verts;
    for (v = 0; v < size[1]; v += step[1])
    {
		for (u = 0; u < size[0]; u += step[0])
		{
			p = v * size[0] + u;
			VectorCopy(vert->v_point, mesh->points[p]);
			Vector4Copy(vert->colour, mesh->colour[p]);
			Vector2Copy(vert->tex_st, mesh->tex_st[p]);
			Vector2Copy(vert->lm_st, mesh->lm_st[p]);
			vert++;
		}
    }

    // Fill in each mesh
    Mesh_FillPatch3(face->mesh_cp, size, mesh->points);
    Mesh_FillPatchC(face->mesh_cp, size, mesh->colour);
    Mesh_FillPatch2(face->mesh_cp, size, (vec2_t *)mesh->tex_st);
    Mesh_FillPatch2(face->mesh_cp, size, (vec2_t *)mesh->lm_st);

    // Allocate and fill element table
    mesh->numelems = (size[0]-1) * (size[1]-1) * 6;
    mesh->elems = (uint_t*)malloc(mesh->numelems * sizeof(uint_t));

    i = 0;
    for (v = 0; v < size[1]-1; v++)
    {
		for (u = 0; u < size[0]-1; u++)
		{
			mesh->elems[i++] = v * size[0] + u;
			mesh->elems[i++] = (v+1) * size[0] + u;
			mesh->elems[i++] = v * size[0] + u + 1;
			mesh->elems[i++] = v * size[0] + u + 1;
			mesh->elems[i++] = (v+1) * size[0] + u;
			mesh->elems[i++] = (v+1) * size[0] + u + 1;
		}
    }
}
