#ifndef TINYSCM_IF_H
#define TINYSCM_IF_H

namespace venk
{
	void load_scheme(scheme* sc, const std::string& fname);
	SDL_Thread* launch_server(scheme *sc);
}

extern "C"
{
	void hold_global_lock();
	void release_global_lock();
}

#endif
