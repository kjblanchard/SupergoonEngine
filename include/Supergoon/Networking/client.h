#pragma once

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
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

typedef void (*NetReceiveCallback)(const void* data, size_t size, void* ctx);

NetClient* NetClientCreate();
void NetClientConnect(NetClient* c, const char* d, int port);
void NetClientUpdate(NetClient* c);
void NetClientSend(NetClient* c, void* d, size_t s);
void NetClientReceive(NetClient* c, NetReceiveCallback cb, void* ctx);
void NetClientDestroy(NetClient* c);
#ifdef __cplusplus
}
#endif
