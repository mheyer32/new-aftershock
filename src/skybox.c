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


/////////// TODO /////////////// TODO 

#include "cmap.h"
#include "skybox.h"

#define SIDE_SIZE 9
#define POINTS_LEN (SIDE_SIZE*SIDE_SIZE)
#define ELEM_LEN ((SIDE_SIZE-1)*(SIDE_SIZE-1)*6)

#define SPHERE_RAD  10.0
#define EYE_RAD      9.0

#define SCALE_S		4.0  // Arbitrary (?) texture scaling factors
#define SCALE_T		4.0 

#define BOX_SIZE	1.0f
#define BOX_STEP	0.25f

static void Gen_BoxSide(int side, vec3_t orig, vec3_t drow, vec3_t dcol);
static void Gen_Box(void);
static void Gen_Elems(void);

void SkyboxCreate (void)
{
    int i;

    // Alloc space for skybox verts, etc.
    r_skybox = (skybox_t *)malloc(sizeof(skybox_t));
    r_skybox->points[0] = (vec3_t *)malloc(5 * POINTS_LEN * sizeof(vec3_t));
    r_skybox->tex_st[0] = (vec2_t *)malloc(5 * POINTS_LEN * sizeof(vec2_t));
    r_skybox->elems = (uint_t *)malloc(ELEM_LEN * sizeof(uint_t));

    r_skybox->numpoints = POINTS_LEN;
    r_skybox->numelems = ELEM_LEN;
    
    for (i = 1; i < 5; i++)
    {
		r_skybox->points[i] = r_skybox->points[i-1] + POINTS_LEN;
		r_skybox->tex_st[i] = r_skybox->tex_st[i-1] + POINTS_LEN;
    }

    Gen_Box();
    Gen_Elems();
}
    
void SkyboxFree (void)
{
    free(r_skybox->points[0]);
    free(r_skybox->tex_st[0]);
    free(r_skybox->elems);
    free(r_skybox);    
}

static void Gen_Elems (void)
{
    int u, v;
    uint_t *e = r_skybox->elems;

    // Box elems in tristrip order
    for (v = 0; v < SIDE_SIZE-1; v++)
    {
		for (u = 0; u < SIDE_SIZE-1; u++)
		{
			*e++ = v * SIDE_SIZE + u;
			*e++ = (v+1) * SIDE_SIZE + u;
			*e++ = v * SIDE_SIZE + u + 1;
			*e++ = v * SIDE_SIZE + u + 1;
			*e++ = (v+1) * SIDE_SIZE + u;
			*e++ = (v+1) * SIDE_SIZE + u + 1;	    
		}
    }
}
    
static void Gen_Box (void)
{
    vec3_t orig, drow, dcol;
    
    // Top
    orig[0] = -BOX_SIZE;
    orig[1] = BOX_SIZE;
    orig[2] = BOX_SIZE;
    drow[0] = 0.0f;
    drow[1] = -BOX_STEP;
    drow[2] = 0.0f;
    dcol[0] = BOX_STEP;
    dcol[1] = 0.0f;
    dcol[2] = 0.0f;
    Gen_BoxSide(SKYBOX_TOP, orig, drow, dcol);

    // Front
    orig[0] = BOX_SIZE;
    orig[1] = BOX_SIZE;
    orig[2] = BOX_SIZE;
    drow[0] = 0.0f;
    drow[1] = 0.0f;
    drow[2] = -BOX_STEP;
    dcol[0] = -BOX_STEP;
    dcol[1] = 0.0f;
    dcol[2] = 0.0f;
    Gen_BoxSide(SKYBOX_FRONT, orig, drow, dcol);

    // Right
    orig[0] = BOX_SIZE;
    orig[1] = -BOX_SIZE;
    orig[2] = BOX_SIZE;
    drow[0] = 0.0f;
    drow[1] = 0.0f;
    drow[2] = -BOX_STEP;
    dcol[0] = 0.0f;
    dcol[1] = BOX_STEP;
    dcol[2] = 0.0f;
    Gen_BoxSide(SKYBOX_RIGHT, orig, drow, dcol);

    // Back
    orig[0] = -BOX_SIZE;
    orig[1] = -BOX_SIZE;
    orig[2] = BOX_SIZE;
    drow[0] = 0.0f;
    drow[1] = 0.0f;
    drow[2] = -BOX_STEP;
    dcol[0] = BOX_STEP;
    dcol[1] = 0.0f;
    dcol[2] = 0.0f;
    Gen_BoxSide(SKYBOX_BACK, orig, drow, dcol);

    // Left
    orig[0] = -BOX_SIZE;
    orig[1] = BOX_SIZE;
    orig[2] = BOX_SIZE;
    drow[0] = 0.0f;
    drow[1] = 0.0f;
    drow[2] = -BOX_STEP;
    dcol[0] = 0.0f;
    dcol[1] = -BOX_STEP;
    dcol[2] = 0.0f;
    Gen_BoxSide(SKYBOX_LEFT, orig, drow, dcol);
}

/*
================
Gen_BoxSide

I don't know exactly what Q3A does for skybox 
texturing, but this is at least fairly close.  
We tile the texture onto the inside of a large 
sphere, and put the camera near the top of the sphere.
We place the box around the camera, and cast rays
through the box verts to the sphere to find the 
texture coordinates.
================
*/
static void Gen_BoxSide (int side, vec3_t orig, vec3_t drow, vec3_t dcol)
{
    vec3_t pos, w, row, *v;
    vec2_t *tc;
    int r, c;
    float d, b, t;

    d = EYE_RAD;     // Sphere center to camera distance
    b = SPHERE_RAD;  // Sphere radius
    
    v = &r_skybox->points[side][0];
    tc = &r_skybox->tex_st[side][0];
    VectorCopy(orig, row);

    for (r = 0; r < SIDE_SIZE; r++)
    {
		VectorCopy(row, pos);

		for (c = 0; c < SIDE_SIZE; c++)
		{
			// pos points from eye to vertex on box
			VectorCopy(pos, (*v));
			VectorCopy(pos, w);

			// Normalize pos -> w
			VectorNormalize (w);

			// Find distance along w to sphere
			t = sqrt(d*d*(w[2]*w[2]-1.0) + b*b) - d*w[2];
			w[0] *= t;
			w[1] *= t;

			t = 1 / (2.0 * SCALE_S);

			// Use x and y on sphere as s and t
			(*tc)[0] = w[0] * t;
			(*tc)[1] = w[1] * t;
			
			VectorAdd(pos, dcol, pos);
			v++;
			tc++;
		}

		VectorAdd(row, drow, row);
    }
}
