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



// From GOLK :www.digital-phenomenon.de


#include "a_shared.h"
#include "console.h"		/* Con_Printf */
#include "net_msg.h"

sizebuf_t		net_message;

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

static int dword_629bbc; 

void MSG_WriteBits( sizebuf_t *buf, int bits, int n )
{
	int				sign;
	int				num_bits;
	unsigned char	*bitsptr;

	if( (buf->maxsize-buf->cursize) < 4 ) {
		buf->overflowed = 1;
		return;
	}

	if( !n || n < -31 || n > 32 ) {
		Error( "MSG_WriteBits: bad bits %i", n );
	}

	if( n!=32 ) {
		if( n > 0 ) {
			sign = (1 << n) - 1;
			if( bits > sign || bits < 0 ) {
				dword_629bbc++;
			}
		} else {
			sign = (1 << (n-1));
			if( bits > (sign-1) || bits < -sign ) {
				dword_629bbc++;
			}
		}
	}

	if( n < 0 ) {
		n = -n;
	}

	if( n ) {

		do {

			if( !buf->bit_offset ) {
				buf->data[buf->cursize] = 0;
				buf->cursize++;
			}

			num_bits = 8 - buf->bit_offset;
			if( num_bits > n ) {
				num_bits = n;
			}

			bitsptr = buf->data + buf->cursize - 1;
			bitsptr[0] = ((bits & (1 << num_bits) - 1) << buf->bit_offset) | bitsptr[0];
			bits >>= num_bits;

			n -= num_bits;
			num_bits = (num_bits + buf->bit_offset) & 7;
			buf->bit_offset = num_bits;

		} while( n );

	}
}

void MSG_WriteChar( sizebuf_t *buf, int c )
{
	MSG_WriteBits( buf, c, -8 );
}

void MSG_WriteByte( sizebuf_t *buf, int c )
{
	MSG_WriteBits( buf, c, 8 );
}

void MSG_WriteShort( sizebuf_t *buf, int c )
{
	MSG_WriteBits( buf, c, 16 );
}

void MSG_WriteLong( sizebuf_t *buf, int c )
{
	MSG_WriteBits( buf, c, 32 );
}

void MSG_WriteFloat( sizebuf_t *buf, float f )
{
	union {
		float	f;
		int		l;
	} dat;
	
	
	dat.f = f;
	dat.l = LittleLong( dat.l );
	
	MSG_Write( buf, &dat.l, 4 );
}

void MSG_WriteString( sizebuf_t *buf, char *s )
{
	if( !s ) {
		MSG_Write( buf, "", 1 );
	} else {
		if( strlen(s) >= MAX_STRING_CHARS ) {
			Error( "MSG_WriteString: MAX_STRING_CHARS" );
		} else {
			MSG_Write( buf, s, strlen(s)+1 );
		}
	}
}

void MSG_WriteCoord( sizebuf_t *buf, float f )
{
	MSG_WriteShort( buf, (int)(f*8) );
}

void MSG_WriteAngle( sizebuf_t *buf, float f )
{
	MSG_WriteByte( buf, ((int)f*256/360) & 255 );
}

//
// reading functions
//
aboolean	msg_badread;

void MSG_BeginReading( sizebuf_t *buf )
{
	buf->bit_offset = 0;
	buf->byte_offset = 0;
	msg_badread = afalse;
}

void MSG_BeginWriting( sizebuf_t *buf )
{
	buf->overflowed = 0;
	buf->cursize = 0;
	buf->bit_offset = 0;
}

// returns -1 and sets msg_badread if no more bits are available
int MSG_ReadBits( sizebuf_t *buf, int count )
{
	int			bitmask;
	int			bits_read;
	aboolean	extend_sign;
	int			num_bits;
	int			max_bits;
	int			sbit;
	int			signbits;

	bitmask = 0;
	bits_read = 0;

	if( count < 0 ) {
		count = -count;
		extend_sign = 1;
	} else {
		extend_sign = 0;
	}

	if( count > 0 ) {
		do {
			if( !buf->bit_offset ) {
				if( buf->byte_offset+1 > buf->cursize ) {
					msg_badread = atrue;
					return( -1 );
				}
				buf->byte_offset++;
			}

			num_bits = 8 - buf->bit_offset;
			max_bits = count - bits_read;
			if( num_bits > max_bits )
				num_bits = max_bits;

			bitmask |= (((buf->data[buf->byte_offset-1] >> buf->bit_offset) & ((1 << num_bits) - 1)) << bits_read);

			buf->bit_offset = (buf->bit_offset + num_bits) & 7;
			bits_read += num_bits;
		} while( bits_read < count );
	}

	if( extend_sign ) {
		sbit = 1 << (count - 1);
		if( bitmask & sbit ) {
			signbits = ~((1 << count) - 1);
			bitmask |= signbits;
		}
	}

	return( bitmask );
}

int MSG_ReadChar( sizebuf_t *buf )
{
	if( buf->byte_offset+1 > buf->cursize )
		return( -1 );

	return( MSG_ReadBits( buf, -8 ) );
}

int MSG_ReadByte( sizebuf_t *buf )
{
	if( buf->byte_offset+1 > buf->cursize )
		return( -1 );

	return( MSG_ReadBits( buf, 8 ) & 0xFF );
}

int MSG_ReadShort( sizebuf_t *buf )
{
	if( buf->byte_offset+2 > buf->cursize )
		return( -1 );

	return( MSG_ReadBits( buf, 16 ) );
}

int MSG_ReadLong( sizebuf_t *buf )
{
	if( buf->byte_offset+4 > buf->cursize )
		return( -1 );

	return( MSG_ReadBits( buf, 32 ) );
}

float MSG_ReadFloat( sizebuf_t *buf )
{
	int i;

	if( buf->byte_offset+4 > buf->cursize )
		return( -1.f );

	i = MSG_ReadBits( buf, 32 );
	return( *((float *)&i) );
}

char *MSG_ReadString( sizebuf_t *buf )
{
	static char	string[2048];
	int			l;
	char		c;
	
	l = 0;
	do {
		c = MSG_ReadChar( buf );
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;

	return( string );
}

float MSG_ReadCoord( sizebuf_t *buf )
{
	return( (float)(MSG_ReadBits( buf, 13 ) - 0x1000) );
}

float MSG_ReadAngle( sizebuf_t *buf )
{
	return( MSG_ReadShort( buf ) * (180.f/32768.f) );
}

void MSG_Checksum( sizebuf_t *buf )
{
	int i;
	int x;

	x = LittleLong(((int *)buf->data)[0]);
	for( i=10; i<buf->cursize; i++ ) {
		x = x*69069 + 1;
		buf->data[i] ^= x;
	}
}

//===========================================================================

void MSG_Alloc( sizebuf_t *buf, int startsize )
{
//	buf->data = (byte *)malloc( startsize );
	buf->maxsize = startsize;
	buf->cursize = 0;
	buf->bit_offset = 0;
	buf->byte_offset = 0;
}

void MSG_Free( sizebuf_t *buf )
{
	//free( buf->data );
	//buf->data = NULL;
	buf->maxsize = 0;
	buf->cursize = 0;
	buf->bit_offset = 0;
	buf->byte_offset = 0;
}

void MSG_Clear( sizebuf_t *buf )
{
	buf->cursize = 0;
	buf->bit_offset = 0;
}

void *MSG_GetSpace( sizebuf_t *buf, int length )
{
	void	*data;
	
	if (buf->cursize + length > buf->maxsize) {
		if (!buf->allowoverflow)
			Error ("MSG_GetSpace: overflow without allowoverflow set");
		
		if (length > buf->maxsize)
			Error ("MSG_GetSpace: %i is > full buffer size", length);
			
		buf->overflowed = atrue;
		Con_Printf( "MSG_GetSpace: overflow" );
		MSG_Clear( buf );
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return( data );
}

void MSG_Write( sizebuf_t *buf, void *data, int length )
{
	memcpy (MSG_GetSpace(buf,length),data,length);
}

void MSG_Print( sizebuf_t *buf, char *data )
{
	int	len;
	
	len = strlen(data)+1;

	// byte * cast to keep VC++ happy
	if (buf->data[buf->cursize-1])
		memcpy ((byte *)MSG_GetSpace(buf, len),data,len); // no trailing 0
	else
		memcpy ((byte *)MSG_GetSpace(buf, len-1)-1,data,len); // write over trailing 0
}
