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
#include "vec.h"
#include <math.h>
#include "util.h"

/* Stolen from Mesa:matrix.c */
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]
/*
void
mat4_mmult(mat4_t a, mat4_t b, mat4_t product)
{
   int i;
   for (i = 0; i < 4; i++)
   {
      float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }

//Faster ???
/*
	  product [0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
	  product[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
	  product[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
	  product[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
	
      product[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
      product[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
      product[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
      product[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
	
      product[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
      product[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
      product[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
      product[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	
      product[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];
      product[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];
      product[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];
      product[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

// ASM ??? 3D NOW ???


*/


//}

/*
void
mat4_vmult(mat4_t a, vec4_t b, vec4_t product)
{
    //int i;
    float b0=b[0], b1=b[1], b2=b[2], b3=b[3];
  /* for (i=0; i < 4; i++)
    {
	product[i] = A(i,0)*b0 + A(i,1)*b1 + A(i,2)*b2 + A(i,3)*b3;
    }
	*/
	/*product[0] = A(0,0)*b0 + A(0,1)*b1 + A(0,2)*b2 + A(0,3)*b3;
	product[1] = A(1,0)*b0 + A(1,1)*b1 + A(1,2)*b2 + A(1,3)*b3;
	product[2] = A(2,0)*b0 + A(2,1)*b1 + A(2,2)*b2 + A(2,3)*b3;
	product[3] = A(3,0)*b0 + A(3,1)*b1 + A(3,2)*b2 + A(3,3)*b3;
	*/
/*
	product[0] = a[0]*b0 + a[4]*b1 + a[8]*b2 + a[12]*b3;
	product[1] = a[1]*b0 + a[5]*b1 + a[9]*b2 + a[13]*b3;
	product[2] = a[2]*b0 + a[6]*b1 + a[10]*b2 + a[14]*b3;
	product[3] = a[3]*b0 + a[7]*b1 + a[11]*b2 + a[15]*b3;

}
*/
/*
void mat4_vmult2 (mat4_t a, vec2_t b, vec2_t product)
{
	float b0=b[0], b1=b[1];
  
	product[0] = A(0,0)*b0 + A(0,1)*b1 + A(0,2)+ A(0,3);
	product[1] = A(1,0)*b0 + A(1,1)*b1 + A(1,2)+ A(1,3);
    


}


void setSigns( cplane_t *in)
{
	int j,bits;
	in->type=4;
	for (j=0;j<3;j++)
	{
		if (in->normal[j]==1.0f)
		{
			in->type=j;
			return;
		}

	}

	bits = 0;
	for (j=0 ; j<3 ; j++) {
		if (in->normal[j] < 0) {
			bits |= 1<<j;
		}
	}
	in->signbits = bits;




}
*/


