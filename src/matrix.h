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

#ifndef MATRIX_H__
#define MATRIX_H__

void Matrix4_Multiply(mat4_t a, mat4_t b, mat4_t product);
void Matrix4_MultiplyFast(mat4_t b, mat4_t c, mat4_t a);
void Matrix_Multiply_Vec4 (mat4_t a, vec4_t b, vec4_t product);
void Matrix_Multiply_Vec2 (mat4_t a, vec2_t b, vec2_t product);
void Matrix4_Transponse (mat4_t m, mat4_t ret);

void Matrix3_Identity (vec3_t mat[3]);
void Matrix3_Multiply (vec3_t in1[3], vec3_t in2[3], vec3_t out[3]);
void Matrix3_Transponse (vec3_t in[3], vec3_t out[3]);

void Matrix_Multiply_Vec3 (mat4_t a, vec3_t b, vec3_t product);

aboolean Matrix4_Inverse (mat4_t mr, mat4_t ma);

#endif 