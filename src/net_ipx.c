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
#include "console.h"		/* Con_Printf */
#include "c_var.h"			/* cvar_t */
#include "network.h"		/* Net_SendPacket */


extern cvar_t *net_port;

NET_SOCKET IPX_Socket(unsigned short port)
{
	NET_SOCKET new_socket;
	struct sockaddr_ipx addr;
	int opt = 1;

	new_socket = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);
	if (new_socket == NET_INVALID_SOCKET) {
		Con_Printf("WARNING: IPX_Socket: socket: %s\n", Net_GetErrorString());
		return(NET_INVALID_SOCKET);
	}
	
	if(ioctlsocket(new_socket, FIONBIO, &opt) == NET_SOCKET_ERROR) {
		Con_Printf("WARNING: IPX_Socket: ioctl FIONBIO: %s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}

	if(setsockopt(new_socket, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt))) {
		Con_Printf("WARNING: IPX_Socket: setsockopt SO_BROADCAST: %s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}
	
	addr.sa_family = AF_IPX;
	memset(addr.sa_netnum,0, 4);
	memset(addr.sa_nodenum,0, 6);
	addr.sa_socket = htons(port);

	if (bind(new_socket, (struct sockaddr *) &addr,  sizeof(addr))==NET_INVALID_SOCKET) {
		Con_Printf("WARNING: IPX_Socket: bind: %s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}

	return(new_socket);
}

aboolean IPX_GetLocalhost(NET_SOCKET socket)
{
	struct sockaddr_ipx	ipx_addr;
	int					len = sizeof(ipx_addr);

	if (getsockname(socket, (struct sockaddr *)&ipx_addr, &len) == NET_SOCKET_ERROR) {
		Con_Printf("getsockname failed\n");
		return(afalse);
	} else {
/*		Con_Printf("");
		memcpy(net_addr, ipx_addr.sa_netnum, 4);
		memcpy(node_addr, ipx_addr.sa_nodenum, 6);
*/		return(atrue);
	}
}

void IPX_AddrToString(net_address_t *addr, char *buf, int buflen)
{
	struct sockaddr_ipx *ipx_addr = (struct sockaddr_ipx *)addr;

	Com_sprintf(buf, buflen, "%02x%02x%02x%02x.%02x%02x%02x%02x%02x%02x:%i",
		ipx_addr->sa_netnum[0] & 0xff,
		ipx_addr->sa_netnum[1] & 0xff,
		ipx_addr->sa_netnum[2] & 0xff,
		ipx_addr->sa_netnum[3] & 0xff,
		ipx_addr->sa_nodenum[0] & 0xff,
		ipx_addr->sa_nodenum[1] & 0xff,
		ipx_addr->sa_nodenum[2] & 0xff,
		ipx_addr->sa_nodenum[3] & 0xff,
		ipx_addr->sa_nodenum[4] & 0xff,
		ipx_addr->sa_nodenum[5] & 0xff,
		ntohs(ipx_addr->sa_socket)
	);
}

aboolean IPX_Broadcast(char *buf, int size, unsigned short port)
{
	struct sockaddr_ipx ipx_addr;

	ipx_addr.sa_family = PF_IPX;
	memset(ipx_addr.sa_netnum,0, 4);
	memset(ipx_addr.sa_nodenum, 0xFF, 6);
	ipx_addr.sa_socket = htons(port);

	return(Net_SendPacket(buf, size, (net_address_t *)&ipx_addr));
}

int IPX_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	if (addr1->sa_family != addr2->sa_family)
		return -1;

	if (*((struct sockaddr_ipx *)addr1)->sa_netnum && *((struct sockaddr_ipx *)addr2)->sa_netnum)
		if (memcmp(((struct sockaddr_ipx *)addr1)->sa_netnum, ((struct sockaddr_ipx *)addr2)->sa_netnum, 4) != 0)
			return -1;
	if (memcmp(((struct sockaddr_ipx *)addr1)->sa_nodenum, ((struct sockaddr_ipx *)addr2)->sa_nodenum, 6) != 0)
		return -1;

	if (((struct sockaddr_ipx *)addr1)->sa_socket != ((struct sockaddr_ipx *)addr2)->sa_socket)
		return 1;

	return 0;
}

int IPX_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	if (addr1->sa_family != addr2->sa_family)
		return -1;

	if (*((struct sockaddr_ipx *)addr1)->sa_netnum && *((struct sockaddr_ipx *)addr2)->sa_netnum)
		if (memcmp(((struct sockaddr_ipx *)addr1)->sa_netnum, ((struct sockaddr_ipx *)addr2)->sa_netnum, 4) != 0)
			return -1;
	if (memcmp(((struct sockaddr_ipx *)addr1)->sa_nodenum, ((struct sockaddr_ipx *)addr2)->sa_nodenum, 6) != 0)
		return -1;

	return 0;
}
