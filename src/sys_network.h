#ifndef SYS_NETWORK_H_INCLUDED
#define SYS_NETWORK_H_INCLUDED

#if _MSC_VER > 1000
# pragma once
#endif

#ifdef _WIN32
# include <winsock2.h>
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

char *Net_GetErrorString(void);
int Net_GetError(void);

#endif