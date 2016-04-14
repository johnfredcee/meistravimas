
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <array>
#include <iostream>
#include <cctype>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_net.h>
#include <res_path.h>
#include <scheme-defs.h>
#include <scheme-private.h>
#include <scheme.h>

#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "tinyscm_if.h"
#include "scripting.h"

namespace venk
{

scheme ScriptingService::tinyscm;
SDL_Thread* ScriptingService::server_thread = nullptr;

bool ScriptingService::initialise(ScriptingService* self)
{
	(void) self;
	std::array<std::string,2> scheme_files = { "init", "image" };
	scheme* sc = &tinyscm;
	int scheme_ok =  scheme_init_custom_alloc(sc, SDL_malloc, SDL_free);
	if(!scheme_ok) {
		std::cerr << "Tiny scheme init failed" << std::endl;
		return false;
	}
	scheme_set_output_port_file(sc,stdout);
	scheme_set_input_port_file(sc,stdin);	
	server_thread = launch_server(sc);
	for(auto const &f : scheme_files) {
		load_scheme(sc, f.c_str());
	}	
	if (server_thread == nullptr) {
		std::cerr << "Server failed to start " << std::endl;
		return false;
	}
	return false;
}

bool ScriptingService::shutdown(ScriptingService* self)
{
	(void) self;
	int server_exit;
	kill_server();
	SDL_WaitThread(server_thread, &server_exit);	
	scheme_deinit(&tinyscm);	
    return true;
}

void ScriptingService::load(const std::string& fileName)
{
	load_scheme(&tinyscm, fileName.c_str());
}

}
