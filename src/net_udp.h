

#ifndef _NET_UDP__
#define _NET_UDP__



NET_SOCKET UDP_OpenSocket(unsigned short port);
aboolean UDP_GetLocalhost(void);
void UDP_AddrToString (net_address_t *addr, char *buf, int buflen);
aboolean UDP_Broadcast(char *buf, int size, unsigned short port);
int UDP_CompareAddr( const net_address_t *addr1, const net_address_t *addr2 );
int UDP_CompareBaseAddr( const net_address_t *addr1, const net_address_t *addr2 );















#endif 