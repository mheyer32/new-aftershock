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