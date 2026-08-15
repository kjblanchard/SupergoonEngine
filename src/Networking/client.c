#include <Supergoon/Networking/client.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sgtools/log.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdlib.h>

void setSocketToNonBlocking(int fd) {
#ifdef _WIN32
  unsigned long mode = blocking ? 0 : 1;
  return (ioctlsocket(fd, FIONBIO, &mode) == 0);
#else
  int flags = fcntl(fd, F_GETFL, 0);
  flags = flags | O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
#endif
}

NetClient *NetClientCreate() {
  int sockedFileDescriptor = 0;
  if ((sockedFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    sgLogError("Socket creation error!");
    return NULL;
  }
  NetClient *c = malloc(sizeof(*c));
  c->SocketFileDescriptor = sockedFileDescriptor;
  setSocketToNonBlocking(c->SocketFileDescriptor);
  return c;
}

void NetClientConnect(NetClient *c, const char *d, int port) {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, d, &serv_addr.sin_addr) <= 0) {
    sgLogError("\nInvalid address/ Address not supported \n");
    return;
  }
  int result = connect(c->SocketFileDescriptor, (struct sockaddr *)&serv_addr,
                       sizeof(serv_addr));
  if (result == 0) {
    c->ConnectionStatus = NET_CONNECTED;
    return;
  }
  if (errno == EINPROGRESS) {
    c->ConnectionStatus = NET_CONNECTING;
    return;
  }
  c->ConnectionStatus = NET_CONNECTION_FAILED;
}

void NetClientUpdate(NetClient *c) {
  if (c->ConnectionStatus != NET_CONNECTING)
    return;
  struct pollfd pfd = {.fd = c->SocketFileDescriptor, .events = POLLOUT};
  int result = poll(&pfd, 1, 0);

  if (result < 0) {
    c->ConnectionStatus = NET_CONNECTION_FAILED;
    return;
  }

  if (result == 0) {
    // Still connecting.
    return;
  }

  if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
    c->ConnectionStatus = NET_CONNECTION_FAILED;
    return;
  }

  if (pfd.revents & POLLOUT) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(c->SocketFileDescriptor, SOL_SOCKET, SO_ERROR, &error,
                   &len) < 0) {
      c->ConnectionStatus = NET_CONNECTION_FAILED;
      return;
    }
    if (error == 0) {
      c->ConnectionStatus = NET_CONNECTED;
    } else {
      c->ConnectionStatus = NET_CONNECTION_FAILED;
    }
  }
}

void NetClientSend(NetClient *c, void *d, size_t s) {
  send(c->SocketFileDescriptor, d, s, 0);
  sgLogInfo("Hello message sent from c\n");
};

void NetClientReceive(NetClient *c) {
  char buffer[1024] = {0};
  read(c->SocketFileDescriptor, buffer, 1024);
}

void NetClientDestroy(NetClient *c) {
  close(c->SocketFileDescriptor);
  free(c);
}
