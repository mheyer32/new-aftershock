
#ifndef _NET_IPX__
#define _NET_IPX__


NET_SOCKET IPX_Socket(unsigned short port);
aboolean IPX_GetLocalhost(NET_SOCKET socket);
void IPX_AddrToString(net_address_t *addr, char *buf, int buflen);
aboolean IPX_Broadcast(char *buf, int size, unsigned short port);
int IPX_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 );
int IPX_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 );







#endif