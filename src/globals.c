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

// REMOVE THIS !!!!

#include "a_shared.h"
#include "cmap.h"
#include "render.h"
#include "tex.h"
#include "mesh.h"
#include "shader.h"
#include "skybox.h"
#include "entity.h"
#include "md3.h"
#include "mapent.h"

vec3_t r_eyepos = {1090, -158, -208};
vec3_t r_eyedir;
float r_eye_az, r_eye_el;
float r_eyefov;
float r_aspect;
float r_sinfov2;
float r_cosfov2;
float r_tanfov;
int r_eyecluster;

float sv_frametime = 0.0;
float cl_frametime = 0.0;
float shadertime = 0.0;

uint_t *r_lightmaptex;
int r_numlightmaptex;

float r_subdivisiontol = 20;
int r_maxmeshlevel = 4;
int r_nummeshes;
mesh_t *r_meshes;
int *r_facemeshes;

int r_numtextures;
shader_t *r_shaders;

skybox_t *r_skybox; 

float r_sinfov;
float r_cosfov;


