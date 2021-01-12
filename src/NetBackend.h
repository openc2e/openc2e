#pragma once

#include "socket.h"

class NetBackend {
  public:
	NetBackend();
	~NetBackend();
	int init();
	void handleEvents();
	void shutdown();

  private:
	bool networkingup = false;
	SOCKET listensocket = INVALID_SOCKET;
};