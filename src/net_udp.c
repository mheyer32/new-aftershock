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
#include "sys_network.h"
#include "network.h"	/* NET_SOCKET */
#include "console.h"		/* Con_Printf */
#include "c_var.h"			/* cvar_t */


// From GOLK :www.digital-phenomenon.de

extern cvar_t *net_port;

NET_SOCKET UDP_OpenSocket(unsigned short port)
{
	NET_SOCKET new_socket;
	struct sockaddr_in addr;
	int opt = 1;

	Con_Printf("Opening IP socket: localhost:%i\n", port);

	new_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (new_socket == NET_INVALID_SOCKET) {
		Con_Printf(S_COLOR_YELLOW "WARNING: UDP_OpenSocket: socket: %s\n", Net_GetErrorString());
		return(NET_INVALID_SOCKET);
	}

	if(ioctlsocket(new_socket, FIONBIO, &opt) == NET_SOCKET_ERROR) {
		Con_Printf(S_COLOR_YELLOW "WARNING: UDP_OpenSocket: ioctl FIONBIO:%s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}

	if(setsockopt(new_socket, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt))) {
		Con_Printf(S_COLOR_YELLOW "WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(new_socket, (struct sockaddr *) &addr,  sizeof(addr))==NET_INVALID_SOCKET) {
		Con_Printf("WARNING: UDP_OpenSocket: bind: %s\n", Net_GetErrorString());
		closesocket(new_socket);
		return(NET_INVALID_SOCKET);
	}

	return(new_socket);
}

aboolean UDP_GetLocalhost(void)
{
	int		i;
	char	buf[256];
	struct	hostent *hp;

	if (!gethostname(buf, sizeof(buf))) {
		hp = gethostbyname(buf);
		if (hp) {
			if (hp->h_addrtype != AF_INET) Con_Printf("gethostbyname: address type was not AF_INET\n");
			Con_Printf("Hostname: %s\n", hp->h_name);
			i = 0;
			while (hp->h_aliases[i]) {
				Con_Printf("Alias: %s\n", hp->h_aliases[i]);
				i++;
			}
			Con_Printf("IP: %i.%i.%i.%i\n",
					(unsigned char)hp->h_addr_list[0][0],
					(unsigned char)hp->h_addr_list[0][1],
					(unsigned char)hp->h_addr_list[0][2],
					(unsigned char)hp->h_addr_list[0][3]);

			return (atrue);
		}
	}
	return (afalse);
}

void UDP_AddrToString (net_address_t *addr, char *buf, int buflen)
{
	struct sockaddr_in *udp_addr = (struct sockaddr_in *)addr;
	int haddr;

	haddr = ntohl(udp_addr->sin_addr.s_addr);
	Com_sprintf(buf, buflen, "%i.%i.%i.%i:%i", (haddr >> 24) & 0xff, (haddr >> 16) & 0xff, (haddr >> 8) & 0xff, haddr & 0xff, ntohs(udp_addr->sin_port));
}

aboolean UDP_Broadcast(char *buf, int size, unsigned short port)
{
	struct sockaddr_in udp_addr;

	udp_addr.sin_family = PF_INET;
	udp_addr.sin_addr.s_addr = INADDR_BROADCAST;
	udp_addr.sin_port = htons(port);

	return(Net_SendPacket(buf, size, (net_address_t *)&udp_addr));
}

int UDP_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	if (addr1->sa_family != addr2->sa_family)
		return -1;

	if (((struct sockaddr_in *)addr1)->sin_addr.s_addr != ((struct sockaddr_in *)addr2)->sin_addr.s_addr)
		return -1;

	if (((struct sockaddr_in *)addr1)->sin_port != ((struct sockaddr_in *)addr2)->sin_port)
		return 1;

	return 0;
}

int UDP_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 )
{
	if (addr1->sa_family != addr2->sa_family)
		return -1;

	if (((struct sockaddr_in *)addr1)->sin_addr.s_addr != ((struct sockaddr_in *)addr2)->sin_addr.s_addr)
		return -1;

	return 0;
}


