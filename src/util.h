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
#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include "vec.h"
/*
typedef unsigned char byte_t;
typedef byte_t coulour_t [4];
*/
/* AIX defines the following in sys/types.h */
#ifndef _AIX
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
#endif

void Error(const char *fmt, ...);


#ifndef M_PI
#define M_PI		3.14159265358979323846	
#endif

#ifdef _WIN32
#pragma warning(disable : 4018)     
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)		
#pragma warning(disable : 4100)		
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)		
#pragma warning(disable : 4127)		
#pragma warning(disable : 4136)
#pragma warning(disable : 4201)
#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)		
#pragma warning(disable : 4310)		
#pragma warning(disable : 4514)
#pragma warning(disable : 4711)		
#pragma warning(disable : 4220)		

#endif
/*

#ifdef ASHOCK_BIG_ENDIAN
#define BYTESWAP(x) x = (((x)&0xff)<<24) | \
                        ((((x)>>8)&0xff)<<16) | \
                        ((((x)>>16)&0xff)<<8) | \
                        ((((x)>>24)&0xff))
#define BYTESWAPSHORT(x) x = (((x)&0xff)<<8) | ((((x)>>8)&0xff))
#define BYTESWAPFLOAT(x) BYTESWAP(*(unsigned *)(&x))
#define BYTESWAPVEC3(x) BYTESWAPFLOAT(x[0]); \
                        BYTESWAPFLOAT(x[1]); \
                        BYTESWAPFLOAT(x[2])
#define BYTESWAPBBOX(x) BYTESWAPFLOAT(x[0]); \
                        BYTESWAPFLOAT(x[1]); \
                        BYTESWAPFLOAT(x[2]); \
                        BYTESWAPFLOAT(x[3]); \
                        BYTESWAPFLOAT(x[4]); \
                        BYTESWAPFLOAT(x[5]);
#else
#define BYTESWAP(x)
#define BYTESWAPSHORT(x)
#define BYTESWAPFLOAT(x)
#define BYTESWAPVEC3(x)
#define BYTESWAPBBOX(x)
#endif
*/




void Error(const char *fmt, ...);
int strcmpEXT (char * str,char * str2 ,int len);
int Search (const char * name ,char * in, char * buf ,int bufsize);

char *copystring(const char *s);



#endif /*__UTIL_H__*/
