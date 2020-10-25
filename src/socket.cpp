#include "socket.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // Windows XP
#endif
#ifndef NTDDI_VERSION
#define NTDDI_VERSION 0x05010000 // Window XP
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

int sockinit() {
#ifdef _WIN32
  WSADATA wsa_data;
  return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
  return 0;
#endif
}

int sockquit() {
#ifdef _WIN32
  return WSACleanup();
#else
  return 0;
#endif
}

int sockgetlasterror() {
#ifdef _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

void sockdestroy(SOCKET sock) {
#ifdef _WIN32
  shutdown(sock, SD_BOTH);
  closesocket(sock);
#else
  shutdown(sock, SHUT_RDWR);
  close(sock);
#endif
}

SOCKET sockcreatetcplistener(uint32_t host, uint16_t port) {
  SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET) {
    return sock;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(host);
  addr.sin_port = htons(port);

  const int bind_result = bind(sock, (sockaddr *)&addr, sizeof(addr));
  if (bind_result != 0) {
    sockdestroy(sock);
    return INVALID_SOCKET;
  }

  const int listen_result = listen(sock, 5);
  if (listen_result != 0) {
    sockdestroy(sock);
    return INVALID_SOCKET;
  }

  return sock;
}

SOCKET sockacceptnonblocking(SOCKET sock) {
#ifdef _WIN32
  unsigned long mode = 1;
  ioctlsocket(sock, FIONBIO, &mode);
#else
  fcntl(sock, F_SETFL, O_NONBLOCK);
#endif

  return accept(sock, NULL, NULL);
}

uint32_t sockgetpeeraddress(SOCKET sock) {
  sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  const int result = getpeername(sock, (sockaddr *)&addr, &addr_len);
  if (result != 0) {
    // TODO: this looks like a real address (0.0.0.0), return error some other way
    return 0;
  }
  return addr.sin_addr.s_addr;
}

int sockrecvblocking(SOCKET sock, char *buf, size_t len, int flags) {
#ifdef WIN32
  unsigned long mode = 0;
  ioctlsocket(sock, FIONBIO, &mode);
#else
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) & ~O_NONBLOCK);
#endif

  return recv(sock, buf, len, flags);
}

int socksendblocking(SOCKET sock, const char *buf, size_t len, int flags) {
#ifdef WIN32
  unsigned long mode = 0;
  ioctlsocket(sock, FIONBIO, &mode);
#else
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) & ~O_NONBLOCK);
#endif

  return send(sock, buf, len, flags);
}