#ifndef SYS_NETWORK_H_INCLUDED
#define SYS_NETWORK_H_INCLUDED

#if _MSC_VER > 1000
# pragma once
#endif

#ifdef _WIN32
#include <winsock2.h>
# include <wsipx.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>		/* timeval */
# include <sys/ioctl.h>		/* ioctl */
# include <netinet/in.h>
# include <netipx/ipx.h>
# include <unistd.h>		/* close */
# include <netdb.h>		/* hostent */
#endif



#ifdef _WIN32
# define NET_SOCKET			SOCKET
# define NET_INVALID_SOCKET		INVALID_SOCKET
# define NET_SOCKET_ERROR		SOCKET_ERROR
# define NET_WOULDBLOCK			WSAEWOULDBLOCK
#else
# define NET_SOCKET			int
# define NET_INVALID_SOCKET		-1
# define NET_SOCKET_ERROR		-1
# define NET_WOULDBLOCK			EWOULDBLOCK
#endif

#ifdef _OS2
# define closesocket	soclose
#elif linux
# define closesocket	close
# define ioctlsocket	ioctl
#endif


#define AFTERSHOCK_PORT 27960


typedef struct net_address_s {
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


char *Net_GetErrorString(void);
int Net_GetError(void);


#endif