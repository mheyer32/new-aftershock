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
#include "matrix.h"

void Matrix4_Identity(mat4_t mat)
{
	memset(mat, 0, 16 * sizeof(float));
	mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

void Matrix3_Identity (vec3_t mat[3])
{
	memset(mat, 0, 9 * sizeof(float));
	mat[0][0] = mat[1][1] = mat[2][2] = 1.0f;
}

void Matrix4_Multiply(mat4_t a, mat4_t b, mat4_t product)
{
	product[0]  = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
	product[1]  = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
	product[2]  = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
	product[3]  = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];
	product[4]  = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
	product[5]  = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
	product[6]  = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
	product[7]  = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];
	product[8]  = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
	product[9]  = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
	product[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
	product[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];
	product[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
	product[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
	product[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	product[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

void Matrix4_MultiplyFast(mat4_t a, mat4_t b, mat4_t product)
{
	product[0]  = a[0] * b[0] + a[4] * b[1] + a[8] * b[2];
	product[1]  = a[1] * b[0] + a[5] * b[1] + a[9] * b[2];
	product[2]  = a[2] * b[0] + a[6] * b[1] + a[10] * b[2];
	product[3]  = 0.0f;
	product[4]  = a[0] * b[4] + a[4] * b[5] + a[8] * b[6];
	product[5]  = a[1] * b[4] + a[5] * b[5] + a[9] * b[6];
	product[6]  = a[2] * b[4] + a[6] * b[5] + a[10] * b[6];
	product[7]  = 0.0f;
	product[8]  = a[0] * b[8] + a[4] * b[9] + a[8] * b[10];
	product[9]  = a[1] * b[8] + a[5] * b[9] + a[9] * b[10];
	product[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10];
	product[11] = 0.0f;
	product[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12];
	product[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13];
	product[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14];
	product[15] = 1.0f;
}

void Matrix3_Multiply (vec3_t in1[3], vec3_t in2[3], vec3_t out[3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}

void Matrix3_Multiply_Vec3 (vec3_t a[3], vec3_t b, vec3_t product)
{
	product[0] = a[0][0]*b[0] + a[1][0]*b[1] + a[2][0]*b[2];
	product[1] = a[0][1]*b[0] + a[1][1]*b[1] + a[2][1]*b[2];
	product[2] = a[0][2]*b[0] + a[1][2]*b[1] + a[2][2]*b[2];
}

// This can be used to calc the inverse of a rotation matrix 
void Matrix3_Transponse (vec3_t in[3], vec3_t out[3])
{
	out[0][0] = in[0][0];
	out[1][1] = in[1][1];
	out[2][2] = in[2][2];

	out[0][1] = in[1][0];
	out[1][0] = in[0][1];

	out[0][2] = in[2][0];
	out[2][0] = in[0][2];
}

void Matrix_Multiply_Vec4 (mat4_t a, vec4_t b, vec4_t product)
{
	product[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2] + a[12]*b[3];
	product[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2] + a[13]*b[3];
	product[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2] + a[14]*b[3];
	product[3] = a[3]*b[0] + a[7]*b[1] + a[11]*b[2] + a[15]*b[3];
}

void Matrix_Multiply_Vec3 (mat4_t a, vec3_t b, vec3_t product)
{
	product[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2] + a[12]*b[3];
	product[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2] + a[13]*b[3];
	product[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2] + a[14]*b[3];
}

// Stolen from Mesa:matrix.c
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

void Matrix_Multiply_Vec2 (mat4_t a, vec2_t b, vec2_t product)
{
	product[0] = a[0]*b[0] + a[1]*b[1] + a[2] + a[3];
	product[1] = a[4]*b[0] + a[5]*b[1] + a[6] + a[7];
}

static float Matrix3_Det (float *mat)
{
    return (mat[0] * (mat[4]*mat[8] - mat[7]*mat[5])
         - mat[1] * (mat[3]*mat[8] - mat[6]*mat[5])
         + mat[2] * (mat[3]*mat[7] - mat[6]*mat[4]));
}

static void Matrix3_Inverse(float *mr, float *ma)
{
	float det = Matrix3_Det ( ma );
	
	if ( fabs( det ) < 0.0005 )
	{
		Matrix3_Identity( (float (*)[3])ma );
		return;
	}

	det = 1 / det;
	
	mr[0] =   ma[4]*ma[8] - ma[5]*ma[7]  * det;
	mr[1] = -(ma[1]*ma[8] - ma[7]*ma[2]) * det;
	mr[2] =   ma[1]*ma[5] - ma[4]*ma[2]  * det;
	
	mr[3] = -(ma[3]*ma[8] - ma[5]*ma[6]) * det;
	mr[4] =   ma[0]*ma[8] - ma[6]*ma[2]  * det;
	mr[5] = -(ma[0]*ma[5] - ma[3]*ma[2]) * det;
	
	mr[6] =   ma[3]*ma[7] - ma[6]*ma[4]  * det;
	mr[7] = -(ma[0]*ma[7] - ma[6]*ma[1]) * det;
	mr[8] =   ma[0]*ma[4] - ma[1]*ma[3]  * det;
}

static void Matrix4_Submat( mat4_t mr, float * mb, int i, int j )
{
    int ti, tj, idst, jdst;
	
    for ( ti = 0; ti < 4; ti++ )
	{
		if ( ti < i )
			idst = ti;
		else
			if ( ti > i )
				idst = ti-1;
			
			for ( tj = 0; tj < 4; tj++ )
			{
				if ( tj < j )
					jdst = tj;
				else
					if ( tj > j )
						jdst = tj-1;
					
					if ( ti != i && tj != j )
						mb[idst*3 + jdst] = mr[ti*4 + tj ];
			}
	}
}

static float Matrix4_Det( mat4_t mr )
{
     float  det, result = 0, i = 1;
     float	msub3[9];
     int    n;

     for ( n = 0; n < 4; n++, i *= -1 )
	 {
		 Matrix4_Submat(mr, msub3, 0, n);
		 
		 det     = Matrix3_Det( msub3 );
		 result += mr[n] * det * i;
	 }
	 
	 return result;
}

aboolean Matrix4_Inverse( mat4_t mr, mat4_t ma )
{
    float  mdet = Matrix4_Det( ma );
    float  mtemp[9];
	int    i, j, sign;
	
	if ( fabs( mdet ) < 0.0005 )
        return 0;
	
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
		{
			sign = 1 - ( (i + j) % 2 ) * 2;
			
			Matrix4_Submat( ma, mtemp, i, j );
			
			mr[i+j*4] = ( Matrix3_Det( mtemp ) * sign ) / mdet;
		}
		
	return 1;
}
