#pragma once

#include <stddef.h>
typedef enum {
	NET_DISCONNECTED,
	NET_CONNECTING,
	NET_CONNECTED,
	NET_CONNECTION_FAILED
} NetConnectionStatus;

typedef struct NetClient {
	int SocketFileDescriptor;
	NetConnectionStatus ConnectionStatus;

} NetClient;

NetClient* NetClientCreate();
void NetClientConnect(NetClient* c, const char* d, int port);
void NetClientUpdate(NetClient* c);
void NetClientSend(NetClient* c, void* d, size_t s);
void NetClientReceive(NetClient* c);
void NetClientDestroy(NetClient* c);
