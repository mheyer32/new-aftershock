
#include "a_shared.h"
#include "matrix.h"


/* Stolen from Mesa:matrix.c */
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]



void Matrix4_Identity(mat4_t mat )
{
	memset(mat,0,16 *sizeof (float));

	mat[0]=mat[5]=mat[10]=mat[15]=1.0;

}

void Matrix3_Identity (vec3_t mat [3])
{
	memset(mat,0,9 *sizeof (float));
	mat[0][0]=mat[1][1]=mat[2][2]=1.0;
}


void Matrix4_Multiply(mat4_t a, mat4_t b, mat4_t product)
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

void Matrix3_Multiply_Vec3 (vec3_t a[3],vec3_t b,vec3_t product)
{
	float b0=b[0], b1=b[1], b2=b[2];

	product[0] = a[0][0]*b0 + a[1][0]*b1 + a[2][0]*b2 ;
	product[1] = a[0][1]*b0 + a[1][1]*b1 + a[2][1]*b2 ;
	product[2] = a[0][2]*b0 + a[1][2]*b1 + a[2][2]*b2 ;


}

// This can be used to calc the inverse of a rotation matrix 
// TODO !!!
void Matrix3_Transponse (vec3_t in [3] ,vec3_t out [3])
{



}


void Matrix_Multiply_Vec4 (mat4_t a, vec4_t b, vec4_t product)
{

    float b0=b[0], b1=b[1], b2=b[2], b3=b[3];

	product[0] = a[0]*b0 + a[4]*b1 + a[8]*b2 + a[12]*b3;
	product[1] = a[1]*b0 + a[5]*b1 + a[9]*b2 + a[13]*b3;
	product[2] = a[2]*b0 + a[6]*b1 + a[10]*b2 + a[14]*b3;
	product[3] = a[3]*b0 + a[7]*b1 + a[11]*b2 + a[15]*b3;

}

void Matrix_Multiply_Vec3 (mat4_t a, vec3_t b, vec3_t product)
{

    float b0=b[0], b1=b[1], b2=b[2], b3=b[3];

	product[0] = a[0]*b0 + a[4]*b1 + a[8]*b2 + a[12]*b3;
	product[1] = a[1]*b0 + a[5]*b1 + a[9]*b2 + a[13]*b3;
	product[2] = a[2]*b0 + a[6]*b1 + a[10]*b2 + a[14]*b3;

}

void Matrix_Multiply_Vec2 (mat4_t a, vec2_t b, vec2_t product)
{
	float b0=b[0], b1=b[1];
  
	product[0] = A(0,0)*b0 + A(0,1)*b1 + A(0,2)+ A(0,3);
	product[1] = A(1,0)*b0 + A(1,1)*b1 + A(1,2)+ A(1,3);
    


}
