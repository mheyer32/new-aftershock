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


#ifndef _NETWORK_H__
#define _NETWORK_H__

#if _MSC_VER > 1000
# pragma once
#endif

#define AFTERSHOCK_PORT 27960


typedef struct  {
	short sa_family;
	unsigned char sa_data[14];
} net_address_t;


aboolean	Net_Init(void);
aboolean	Net_Shutdown(void);
aboolean	Net_Resolve(const char *, net_address_t *);
aboolean	Net_SendPacket(const char *, int, const net_address_t *);
aboolean	Net_GetPacket(char *, int *, net_address_t *);
int			NET_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 );
int			NET_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 );



#endif