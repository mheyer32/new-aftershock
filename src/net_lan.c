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
#include "sys_main.h"
#include "c_var.h"
#include "console.h"
#include "command.h"
#include "sys_network.h"
#include "net_msg.h"
#include "network.h"

#define AFTERSHOCK_PORT 27960

extern cvar_t *net_noudp;
extern cvar_t *net_noipx;
extern cvar_t *protocol;

#define MAX_LOCAL_SERVERS	256
#define MAX_GLOBAL_SERVERS	4096
#define MAX_PINGREQUESTS	16

typedef struct ping_request_s {
	net_address_t		addr;
	int					pingstart;
} ping_request_t;

typedef struct ping_result_s {
	net_address_t		addr;
	int					pingtime;
	char				info[MAX_INFO_STRING];
} ping_result_t;

static net_address_t	localservers[MAX_LOCAL_SERVERS];
static net_address_t	globalservers[MAX_GLOBAL_SERVERS];
static ping_request_t	pingqueue[MAX_PINGREQUESTS];
static ping_result_t	pingresults[MAX_PINGREQUESTS];
static int				num_local_servers = 0;
static int				num_global_servers = 0;
static int				num_pingrequests = 0;
static int				num_pingresults = 0;

void LAN_Ping( void )
{
	char			arg1[MAX_TOKEN_CHARS];
	net_address_t	addr;
	sizebuf_t		buf;
	ping_request_t	*ping;

	if( Cmd_Argc() >= 2 ) {
		Cmd_Argv( 1, arg1, sizeof(arg1) );
		if( !Net_Resolve( arg1, &addr ) )
			return;

		MSG_Alloc( &buf, 64 );
		MSG_BeginWriting( &buf );

		MSG_WriteLong( &buf, -1 );
		MSG_WriteString( &buf, "getinfo xxx" );

		Net_SendPacket( buf.data, buf.cursize, &addr );

		MSG_Free( &buf );

		if( num_pingrequests >= MAX_PINGREQUESTS )
			return;

		/* add to ping queue */
		ping = &pingqueue[num_pingrequests++];
		memcpy( &ping->addr, &addr, sizeof(addr) );
		ping->pingstart = Sys_Get_Time();
	} else {
		Con_Printf( "usage: ping [server]\n" );
	}
}

void LAN_FindLocalServers( void )
{
	unsigned short	port;
	sizebuf_t		buf;

	Con_Printf( "Scanning for servers on the local network...\n" );

	num_local_servers = 0;

	MSG_Alloc( &buf, 64 );
	MSG_BeginWriting( &buf );

	MSG_WriteLong( &buf, -1 );
	MSG_WriteString( &buf, "getinfo xxx" );

	/* broadcast message */
	for( port=AFTERSHOCK_PORT; port<AFTERSHOCK_PORT+4; port++ ) {
		if( !net_noudp->integer )
			UDP_Broadcast( buf.data, buf.cursize, port );
		if( !net_noipx->integer )
			IPX_Broadcast( buf.data, buf.cursize, port );
	}

	MSG_Free( &buf );
}

void LAN_FindGlobalServers( void )
{
	sizebuf_t		outbuf;
	int				argc;
	int				ret;
	char			arg1[MAX_TOKEN_CHARS];
	char			arg2[MAX_TOKEN_CHARS];
	char			arg3[MAX_TOKEN_CHARS];
	char			arg4[MAX_TOKEN_CHARS];
	char			arg5[MAX_TOKEN_CHARS];
	net_address_t	addr;
	char			buf[64];

	argc = Cmd_Argc();
	if( argc < 5 ) {
		Con_Printf( "usage: globalservers <master# 0-1> <protocol> [keywords]\n" );
	}

	Con_Printf( "Requesting servers from the master...\n" );

	Cmd_Argv( 1, arg1, sizeof(arg1) );
	Cmd_Argv( 2, arg2, sizeof(arg2) );
	Cmd_Argv( 3, arg3, sizeof(arg3) );
	Cmd_Argv( 4, arg4, sizeof(arg4) );
	Cmd_Argv( 5, arg5, sizeof(arg5) );

	if( !stricmp( arg1, "0" ) )
		ret = Net_Resolve( "master.quake3arena.com", &addr );
	else
		ret = Net_Resolve( "master.quake3world.com", &addr );

	if( !ret )
		return;

	num_global_servers = 0;

	MSG_Alloc( &outbuf, 256);
	MSG_BeginWriting( &outbuf );

	Com_sprintf( buf, sizeof(buf), "getservers %s %s %s ",arg3,arg4,arg5 );

	MSG_WriteLong( &outbuf, -1 );
	MSG_WriteString( &outbuf, buf );

	Net_SendPacket( outbuf.data, outbuf.cursize, &addr );

	MSG_Free( &outbuf );
}

int LAN_GetLocalServerCount( void )
{
	return( num_local_servers );
}

void LAN_GetLocalServerAddressString( int n, char *buf, int buflen )
{
	net_address_t *addr;

	if( n<0 || n>num_local_servers) {
		A_strncpyz( buf, "", buflen );
		return;
	}

	addr = &localservers[n];
	switch (addr->sa_family) {
		case PF_IPX:
			IPX_AddrToString( addr, buf, buflen );
			break;
		case PF_INET:
			UDP_AddrToString( addr, buf, buflen );
			break;
	}
}

int LAN_GetGlobalServerCount(void)
{
	return( num_global_servers );
}

void LAN_GetGlobalServerAddressString( int n, char *buf, int buflen )
{
	net_address_t *addr;

	if( n < 0 || n > num_global_servers ) {
		A_strncpyz( buf, "", buflen );
		return;
	}

	addr = &globalservers[n];
	switch (addr->sa_family) {
		case PF_IPX:
			IPX_AddrToString( addr, buf, buflen );
			break;
		
		case PF_INET:
			UDP_AddrToString( addr, buf, buflen );
			break;
	}
}

int LAN_GetPingQueueCount( void )
{
	return( num_pingrequests );
}

void LAN_ClearPing( int n )
{
	ping_result_t	*ping;

	if( n < 0 || n >= MAX_PINGREQUESTS )
		return;

	ping = &pingresults[n];
	ping->addr.sa_family = 0;
	ping->info[0] = 0;
	ping->pingtime = 0;
}

void LAN_GetPing( int n, char *buf, int buflen, int *pingtime )
{
	ping_result_t	*ping;

	if( (n < 0) || (n >= MAX_PINGREQUESTS) || (!pingresults[n].pingtime) ) {
		A_strncpyz( buf, "", buflen );
		if( pingtime ) *pingtime = 0;
		return;
	}

	ping = &pingresults[n];

	switch( ping->addr.sa_family ) {
		case PF_INET:
			UDP_AddrToString( &ping->addr, buf, buflen );
			break;
		case PF_IPX:
			IPX_AddrToString( &ping->addr, buf, buflen );
			break;
		default:
			A_strncpyz( buf, "", buflen );
	}

	if( pingtime )
		*pingtime = ping->pingtime;
}

void LAN_GetPingInfo( int n, char *buf, int buflen )
{
	if( (n < 0) || (n >= MAX_PINGREQUESTS) || (!pingresults[n].pingtime) ) {
		A_strncpyz( buf, "", buflen );
		return;
	}

	A_strncpyz( buf, pingresults[n].info, buflen );
}

void LAN_AddServer( net_address_t *addr, const char *info )
{
	ping_request_t	*req;
	ping_result_t	*ping;
	char			addrstring[64];
	int				i;
	int				j;
	int				pingtime;
	const char		*nettype;

	req = pingqueue;
	for( i=0; i<num_pingrequests; i++, req++ ) {
		if( NET_CompareAddr( addr, &req->addr ) )
			continue;

		pingtime = Sys_Get_Time() - req->pingstart;

		switch( addr->sa_family ) {
			case PF_INET:
				UDP_AddrToString( addr, addrstring, sizeof(addrstring) );
				nettype = "UDP";
				break;
			case PF_IPX:
				IPX_AddrToString( addr, addrstring, sizeof(addrstring) );
				nettype = "IPX";
			default:
				nettype = "???";
		}

		Con_Printf( "ping time %dms from %s\n", pingtime, addrstring );

		/* remove request from ping queue */
		num_pingrequests--;
		for( ;i<num_pingrequests; i++ )
			memcpy( &pingqueue[i], &pingqueue[i+1], sizeof(ping_request_t) );

		/* save result */
		ping = pingresults;
		for( j=0; j<MAX_PINGREQUESTS; j++, ping++ ) {
			if( !ping->pingtime ) {
				memcpy( &ping->addr, addr, sizeof(*addr) );
				ping->pingtime = pingtime;
				A_strncpyz( ping->info, info, sizeof(ping->info) );
				Info_SetValueForKey( ping->info, "nettype", nettype );
				break;
			}
		}

		return;
	}

	if( num_local_servers >= MAX_LOCAL_SERVERS ) {
		Con_Printf( "MAX_LOCAL_SERVERS hit, dropping infoResponse\n" );
		return;
	}

	for( i=0; i<num_local_servers; i++ )
		if( !NET_CompareAddr( addr, &localservers[i] ) )
			return;

	/* add to server list */
	memcpy( &localservers[num_local_servers++], addr, sizeof(*addr) );
}
