#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
// mimic winsock2.h
#ifdef _WIN64
typedef unsigned __int64 SOCKET;
#else
typedef unsigned int SOCKET;
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((SOCKET)~0)
#endif
#else
typedef int SOCKET;
#define INVALID_SOCKET ((SOCKET)-1)
#endif

int sockinit();
int sockquit();
int sockgetlasterror();
void sockdestroy(SOCKET sock);
SOCKET sockcreatetcplistener(uint32_t host, uint16_t port);
SOCKET sockacceptnonblocking(SOCKET sock);
uint32_t sockgetpeeraddress(SOCKET sock);
int sockrecvblocking(SOCKET sock, char* buf, size_t len, int flags);
int socksendblocking(SOCKET sock, const char* buf, size_t len, int flags);