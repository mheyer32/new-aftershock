
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
#include "console.h"	/* Con_Printf */
#include "c_var.h"		/* Cvar_Get */
#include "command.h"	/* Cmd_AddCommand */
#include "sys_network.h"
#include "network.h"
#include "net_msg.h"
#include "net_lan.h"

/* TODO: IPX */
#define AFTERSHOCK_PORT 27960

static aboolean SocketOpen = afalse;
static aboolean SocksReady = afalse;
static NET_SOCKET local_socket;

/* cvars */
cvar_t *net_noipx;
cvar_t *net_noudp;
cvar_t *net_port;

typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int	flags;
} cvarTable_t;

static cvarTable_t cvarTable[] = {
	{ &net_noipx, "net_noipx", "0", CVAR_ARCHIVE|CVAR_LATCH },
	{ &net_noudp, "net_noudp", "0", CVAR_ARCHIVE|CVAR_LATCH },
	{ &net_port, "net_port", "27960", CVAR_LATCH }
};

const static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

/*
=================
Net_GetCvars
=================
*/
static void Net_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

static void Cmd_net_restart(void)
{
	Net_Shutdown();
	Net_Init();
}

aboolean Net_Resolve( const char *hostname, net_address_t *addr )
{
	struct hostent		*host;
	int					port;
	char				*name;
	char				*colon;
	struct sockaddr_in	*udp_addr;

	port = net_port->integer;
	name = copystring( hostname );

	colon = A_strrchr( name, ':' );

	/* parse port */
	if( colon ) port = atoi( colon+1 );

	/* parse name */
	if( colon ) *colon = 0;

	/* resolve name */
	host = gethostbyname( name );
	free( name );
	if( host ) {
		if( host->h_addrtype != AF_INET ) {
			Con_Printf( "gethostbyname: address type was not AF_INET\n" );
			return( afalse );
		}

		addr->sa_family = PF_INET;
		udp_addr = (struct sockaddr_in *)addr;
		udp_addr->sin_addr.S_un.S_un_b.s_b1 = (unsigned char)host->h_addr_list[0][0];
		udp_addr->sin_addr.S_un.S_un_b.s_b2 = (unsigned char)host->h_addr_list[0][1];
		udp_addr->sin_addr.S_un.S_un_b.s_b3 = (unsigned char)host->h_addr_list[0][2];
		udp_addr->sin_addr.S_un.S_un_b.s_b4 = (unsigned char)host->h_addr_list[0][3];
		udp_addr->sin_port = htons( port );

		Con_Printf( "%s resolved to %d.%d.%d.%d:%d\n", hostname,
				udp_addr->sin_addr.S_un.S_un_b.s_b1,
				udp_addr->sin_addr.S_un.S_un_b.s_b2,
				udp_addr->sin_addr.S_un.S_un_b.s_b3,
				udp_addr->sin_addr.S_un.S_un_b.s_b4,
				port );
		return( atrue );
	}

	Con_Printf( "Couldn't resolve address\n" );

	return( afalse );
}

/*
 * Send a packet. PktInfo has to be filled out.
 *	address - address to send to
 *	size    - size of the packet
 *	buffer  - pointer to the data
 * Returns true if sent.
 */
aboolean Net_SendPacket( const char *buffer, int size, const net_address_t *address )
{
	int sent;
	
	if(!SocksReady || !SocketOpen) return (afalse);

	sent = sendto( local_socket, buffer, size, 0, (struct sockaddr *) address, sizeof(net_address_t) );
	if( sent == NET_SOCKET_ERROR ) {
		Con_Printf( "ERROR: Net_SendPacket: %s\n", Net_GetErrorString() );
		return( afalse );
	}

	return( sent==size );
}

/*
 * Get a packet from the local aocket. Before the call:
 *	size    - size of the buffer
 *	buffer  - pointer to the data
 * If a packet has been received:
 *	address - address of the sender
 *	size    - size of the packet data
 * Returns: true if a packet has been read.
 */
aboolean Net_GetPacket( char *buffer, int *size, net_address_t *address )
{
	int addrlen;
	int len;

	if(!SocksReady || !SocketOpen) return (afalse);

	addrlen = sizeof(net_address_t);

	len = recvfrom(local_socket, buffer, *size, 0, (struct sockaddr *)address, &addrlen);
	if( len == NET_SOCKET_ERROR ) {
		if( Net_GetError() != NET_WOULDBLOCK ) {
			Con_Printf( "ERROR: Net_GetPacket: %s\n", Net_GetErrorString() );
		}
		*size = 0;
		return( afalse );
	}

	*size = len;

	return( atrue );
}

int NET_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	switch( addr1->sa_family ) {
		case AF_INET:
			return( UDP_CompareAddr( addr1, addr2 ) );
		case AF_IPX:
			return( UDP_CompareAddr( addr1, addr2 ) );
		default:
			Con_Printf( "NET_CompareAddr: bad address type\n" );
			return( -1 );
	}
}

int NET_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	switch( addr1->sa_family ) {
		case AF_INET:
			return( UDP_CompareBaseAddr( addr1, addr2 ) );
		case AF_IPX:
			return( UDP_CompareBaseAddr( addr1, addr2 ) );
		default:
			Con_Printf( "NET_CompareBaseAddr: bad address type\n" );
			return( -1 );
	}
}

aboolean Net_Init( void )
{
#ifdef _WIN32
	WSADATA wsaData;
	int failed;
#endif
	unsigned short port;
	char buf[1024];

	if (SocksReady)
		return (1);

#ifdef _WIN32
	failed = WSAStartup( WINSOCK_VERSION, &wsaData );
	if( failed ) {
		Con_Printf( "WARNING: Winsock initialization failed, returned %d\n", failed );
		return( afalse );
	} else {
		Con_Printf( "Winsock initialized\n" );
	}
#endif

	SocksReady = atrue;

	Cmd_AddCommand( "net_restart", Cmd_net_restart );

	Net_GetCvars();

	if(!net_noudp->integer) {
		for (port=AFTERSHOCK_PORT;port<AFTERSHOCK_PORT+4;port++) {
			local_socket = UDP_OpenSocket(port);
			if(local_socket != NET_INVALID_SOCKET) {
				Com_sprintf( buf, sizeof(buf), "%d", port );
				Cvar_Set2( "net_port", buf,0 );
				UDP_GetLocalhost();
				SocketOpen = atrue;
				break;
			}
		}
	}

#if 0
	if(!net_noipx->integer) {
		for (port=AFTERSHOCK_PORT;port<AFTERSHOCK_PORT+4;port++) {
			local_socket = IPX_Socket(port);
			if(local_socket != NET_INVALID_SOCKET) {
				Com_sprintf( buf, sizeof(buf), "%d", port );
				Cvar_Set2( "net_port", buf );
				IPX_GetLocalhost( local_socket );
				SocketOpen = atrue;
				break;
			}
		}
	}
#endif

	MSG_Alloc( &net_message, NET_MAXMESSAGE );

	return (atrue);
}

aboolean Net_Shutdown(void)
{
	if (!SocksReady)
		return (afalse);

	if (SocketOpen) {
		closesocket(local_socket);
		SocketOpen = afalse;
	}

#ifdef _WIN32
	if (WSACleanup())
		return (afalse);
#endif

	SocksReady = afalse;

	MSG_Free( &net_message );

	return (atrue);
}