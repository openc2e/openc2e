#include "NetBackend.h"

#include "Engine.h"
#include "common/Exception.h"

#include <fmt/core.h>

NetBackend::NetBackend() = default;

NetBackend::~NetBackend() {
	shutdown();
};

int NetBackend::init() {
	const int init_result = sockinit();
	if (init_result != 0) {
		throw Exception(fmt::format("Networking error during initialization: {}", init_result));
	}

	networkingup = true;

	listensocket = INVALID_SOCKET;
	int listenport;
	for (listenport = 20001; listenport < 20050; listenport++) {
		listensocket = sockcreatetcplistener(0, listenport);
		if (listensocket != INVALID_SOCKET) {
			break;
		}
	}

	if (listensocket == INVALID_SOCKET) {
		throw Exception(std::string("Failed to open a port to listen on."));
	}

	return listenport;
}

void NetBackend::handleEvents() {
	if (!networkingup) {
		return;
	}

	// handle incoming network connections
	while (true) {
		SOCKET connection = sockacceptnonblocking(listensocket);
		if (connection == INVALID_SOCKET) {
			break;
		}
		// check this connection is coming from localhost
		uint32_t peer_addr = sockgetpeeraddress(connection);
		unsigned char* rip = (unsigned char*)&peer_addr;
		if ((rip[0] != 127) || (rip[1] != 0) || (rip[2] != 0) || (rip[3] != 1)) {
			fmt::print("Someone tried connecting via non-localhost address! IP: {}.{}.{}.{}\n", rip[0], rip[1], rip[2], rip[3]);
			sockdestroy(connection);
			continue;
		}

		// read the data from the socket
		std::string data;
		bool done = false;
		while (!done) {
			char buffer;
			int i = sockrecvblocking(connection, &buffer, 1, 0);
			if (i == 1) {
				data = data + buffer;
				// TODO: maybe we should check for rscr\n like c2e seems to
				if ((data.size() > 3) && (data.find("rscr\n", data.size() - 5) != data.npos))
					done = true;
			} else
				done = true;
		}

		// pass the data onto the engine, and send back our response
		std::string tosend = engine.executeNetwork(data);
		socksendblocking(connection, tosend.c_str(), tosend.size(), 0);

		// and finally, close the connection
		sockdestroy(connection);
	}
}

void NetBackend::shutdown() {
	if (listensocket != INVALID_SOCKET) {
		sockdestroy(listensocket);
	}
	if (networkingup) {
		sockquit();
	}
	networkingup = false;
}