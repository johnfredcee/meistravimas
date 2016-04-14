#ifndef TINYSCM_IF_H
#define TINYSCM_IF_H

#ifdef __cplusplus
namespace venk
{
	void load_scheme(scheme* sc, const std::string& fname);
	SDL_Thread* launch_server(scheme *sc);
	void kill_server();	
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif	
	void hold_global_lock();
	void release_global_lock();
#ifdef __cplusplus
}
#endif

#endif
