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


#ifndef NET_LAN_H_INCLUDED
#define NET_LAN_H_INCLUDED

void	LAN_FindLocalServers( void );
int		LAN_GetLocalServerCount( void );
void	LAN_GetLocalServerAddressString( int n, char *buf, int buflen );
int		LAN_GetGlobalServerCount( void );
void	LAN_GetGlobalServerAddressString( int n, char *buf, int buflen );
void	LAN_FindGlobalServers( void );
int		LAN_GetPingQueueCount( void );
void	LAN_ClearPing( int n );
void	LAN_GetPing( int n, char *buf, int buflen, int *pingtime );
void	LAN_GetPingInfo( int n, char *buf, int buflen );

void	LAN_Ping( void );
void	LAN_AddServer( struct net_address_t *addr, const char *info );

#endif				/* NET_LAN_H_INCLUDED */
