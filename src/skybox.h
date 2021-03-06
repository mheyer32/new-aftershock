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
#ifndef __SKYBOX_H__
#define __SKYBOX_H__

/* The skybox has 5 sides (no bottom) */
enum
{
    SKYBOX_TOP    = 0,
    SKYBOX_FRONT,
    SKYBOX_RIGHT,
    SKYBOX_BACK,
    SKYBOX_LEFT
};

typedef struct
{
    int			numpoints;
    vec3_t		*points[5];     // World coords
    vec2_t		*tex_st[5];		// Skybox mapped texture coords
    int			numelems;
    uint_t		*elems;
} skybox_t;

extern skybox_t *r_skybox;

void SkyboxCreate(void);
void SkyboxFree(void);

#endif /*_SKYBOX_H__*/
