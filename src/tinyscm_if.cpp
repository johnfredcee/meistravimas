
#include <string>
#include <iostream>
#include <physfs.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_net.h>
#include <scheme-private.h>
#include "res_path.h"

namespace venk {

void load_scheme(scheme* sc, const std::string& fname) {
	std::string realfname = std::string("scheme/") +fname + std::string(".scm");
	Sint32 length = 0;
	char* source = file_contents(realfname.c_str(), &length);
	SDL_assert_always((source != nullptr) && (length > 0));
	if((!source) || (length == 0)) {
		std::cerr << "Could not load " << fname << std::endl;
		return;
	}
	scheme_load_string(sc, source);
	return;
}


int server() {
	TCPsocket sd, csd; /* Socket descriptor, Client socket descriptor */
	IPaddress ip, *remoteIP;
	int quit, quit2;
	char buffer[512];
	if(SDLNet_Init() < 0) {
		std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
		return -1;
	}
	/* Resolving the host using NULL make network interface to listen */
	if(SDLNet_ResolveHost(&ip, nullptr, 2956) < 0) {
		std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
		return -1;
	}
	/* Open a connection with the IP provided (listen on the host's port) */
	if(!(sd = SDLNet_TCP_Open(&ip))) {
		std::cerr << "SDLNet_TCP_Open:" << SDLNet_GetError() << std::endl;
		return -1;
	}
	/* Wait for a connection, send data and term */
	quit = 0;
	while(!quit) {
		/* This check the sd if there is a pending connection.
		* If there is one, accept that, and open a new socket for communicating */
		if((csd = SDLNet_TCP_Accept(sd))) {
			/* Now we can communicate with the client using csd socket
			* sd will remain opened waiting other connections */
			/* Get the remote address */
			if((remoteIP = SDLNet_TCP_GetPeerAddress(csd))) {
				/* Print the address, converting in the host format */
				std::cout << "Host connected: " << std::hex << SDLNet_Read32(&remoteIP->host) << std::dec <<  SDLNet_Read16(&remoteIP->port) << std::endl;
			} else {
				std::cerr << "SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << std::endl;
			}
			quit2 = 0;
			while(!quit2) {
				if(SDLNet_TCP_Recv(csd, buffer, 512) > 0) {
					std::cout << "Client say: " << buffer << std::endl;
					if(strcmp(buffer, "exit") == 0) {	/* Terminate this connection */
						quit2 = 1;
						std::cout << "Terminate connection" << std::endl;
					}
					if(strcmp(buffer, "quit") == 0) {	/* Quit the program */
						quit2 = 1;
						quit = 1;
						std::cout <<"Quit program" << std::endl;
					}
				}
			}
			/* Close the client socket */
			SDLNet_TCP_Close(csd);
		}
	}
	SDLNet_TCP_Close(sd);
	SDLNet_Quit();
	return 0;
}

}

