
#include <string>
#include <iostream>
#include <physfs.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include <scheme-private.h>
#include "res_path.h"

extern "C"
{
	pointer run_test(scheme* sc, pointer args);
}

namespace venk {

void load_scheme(scheme* sc, const std::string& fname) {
	std::string realfname = std::string("scheme/") +fname + std::string(".scm");
	Uint64 length = 0;
	char* source = file_contents(realfname.c_str(), &length);
	SDL_assert_always((source != nullptr) && (length > 0));
	if((source == nullptr) || (length == 0)) {
		std::cerr << "Could not load " << fname << std::endl;
		return;
	}
	SDL_RWops *rwop = SDL_RWFromMem((void*) source, length);		;
	int i = ++(sc->file_i);
	SDL_assert(i < MAXFIL);
	sc->load_stack[i].kind = port_sdl|port_input;
	sc->load_stack[i].rep.sdl.rwop = rwop;
	char *buffer =  (char*) sc->malloc(SOCKET_BUFFER_SIZE);
	if (buffer == NULL) {
		return;
	}
	sc->load_stack[i].rep.sdl.buffer = buffer;
	sc->load_stack[i].rep.sdl.start = buffer;
	sc->load_stack[i].rep.sdl.end = buffer;
	sc->nesting_stack[sc->file_i]=0;
	sc->loadport->_object._port=sc->load_stack+sc->file_i;
	pointer args = mk_integer(sc, sc->file_i);
	pointer proc = mk_proc(sc, OP_T0LVL);
	scheme_call(sc, proc, args);
	return;
}


int server(void *data) {

	scheme *sc = (scheme *) data;
	TCPsocket sd, csd; /* Socket descriptor, Client socket descriptor */
	IPaddress ip, *remoteIP;

	scheme_define(sc,sc->global_env,mk_symbol(sc,"run-test"),mk_foreign_func(sc, run_test));

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
	/* This check the sd if there is a pending connection.
	 * If there is one, accept that, and open a new socket for communicating */
	while((csd = SDLNet_TCP_Accept(sd)) == nullptr) {
		SDL_Delay(1);
	}
	/* Now we can communicate with the client using csd socket
	 * sd will remain opened waiting other connections */
	/* Get the remote address */
	if((remoteIP = SDLNet_TCP_GetPeerAddress(csd))) {
		/* Print the address, converting in the host format */
		std::cout << "Host connected: " << std::hex << SDLNet_Read32(&remoteIP->host) << ":" << std::dec <<  SDLNet_Read16(&remoteIP->port) << std::endl;
	} else {
		std::cerr << "SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << std::endl;
	}
	scheme_load_string(sc, "(write \"SDL Scheme\") (newline)");
	scheme_set_port_net(sc, csd);
	scheme_load_socket(sc, csd);
	std::cout << "Terminate connection" << std::endl;
	/* Close the client socket */
	SDLNet_TCP_Close(csd);
	csd = nullptr;

	/* Close the server socket */
	SDLNet_TCP_Close(sd);
	return 0;
}

SDL_Thread* launch_server(scheme* sc) {
	SDL_Thread* threadID = SDL_CreateThread( server, "SchemePort", (void*) sc );
	return threadID;
}

} // namespace venk

extern SDL_sem* global_lock;

extern "C"
{
	static int lock_held = 0;

	void hold_global_lock() {
		if (!lock_held) {
			SDL_SemWait(global_lock);
		}
		lock_held++;
	}

	void release_global_lock() {
		SDL_assert(lock_held != 0);
		lock_held--;
		if (!lock_held) {
			SDL_SemPost(global_lock);
		}
	}

	pointer run_test(scheme* sc, pointer args) {
		hold_global_lock();
		if (args == sc->NIL)
			return sc->F;
		if (is_string(pair_car(args))) {
			venk::load_scheme(sc, string_value(pair_car(args)));
		}
		release_global_lock();
		return sc->T;
	}
}
