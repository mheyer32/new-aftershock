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


#ifndef NET_MSG_H_INCLUDED
#define NET_MSG_H_INCLUDED

#if _MSC_VER > 1000
# pragma once
#endif

#include "network.h"		/* net_address_t */


#define NET_MAX_PACKET_LEN 16384


//============================================================================

#define NET_MAXMESSAGE		65536

typedef struct sizebuf_s {
	aboolean	allowoverflow;	// if false, do a Sys_Error
	aboolean	overflowed;		// set to true if the buffer size failed
	//byte		*data;

	union {
		unsigned int Sequence_Number ;
		byte data [NET_MAX_PACKET_LEN];
	};
	int			maxsize;
	int			cursize;
	int			byte_offset;
	int			bit_offset;
} sizebuf_t;

void	MSG_Alloc( sizebuf_t *buf, int startsize );
void	MSG_Free( sizebuf_t *buf );
void	MSG_Clear( sizebuf_t *buf );
void	*MSG_GetSpace( sizebuf_t *buf, int length );
void	MSG_Write( sizebuf_t *buf, void *data, int length );
void	MSG_Print( sizebuf_t *buf, char *data );	// strcats onto the sizebuf

void	MSG_BeginWriting( sizebuf_t *buf );
void	MSG_WriteChar( sizebuf_t *buf, int c );
void	MSG_WriteByte( sizebuf_t *buf, int c );
void	MSG_WriteShort( sizebuf_t *buf, int c );
void	MSG_WriteLong( sizebuf_t *buf, int c );
void	MSG_WriteFloat( sizebuf_t *buf, float f );
void	MSG_WriteString( sizebuf_t *buf, char *s );
void	MSG_WriteCoord( sizebuf_t *buf, float f );
void	MSG_WriteAngle( sizebuf_t *buf, float f );
void	MSG_WriteBits( sizebuf_t *buf, int bits, int n );

extern	aboolean	msg_badread;		// set if a read goes beyond end of message

void	MSG_BeginReading( sizebuf_t *buf );
int		MSG_ReadChar( sizebuf_t *buf );
int		MSG_ReadByte( sizebuf_t *buf );
int		MSG_ReadShort( sizebuf_t *buf );
int		MSG_ReadLong( sizebuf_t *buf );
float	MSG_ReadFloat( sizebuf_t *buf );
char *	MSG_ReadString( sizebuf_t *buf );
float	MSG_ReadCoord( sizebuf_t *buf );
float	MSG_ReadAngle( sizebuf_t *buf );
int		MSG_ReadBits( sizebuf_t *buf, int count );

extern sizebuf_t		net_message;

#endif				/* NET_MSG_H_INCLUDED */
