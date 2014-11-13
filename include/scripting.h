#ifndef SCRIPTING_H
#define SCRIPTING_H

namespace venk {
	
class ScriptingService : public Service<ScriptingService> {
public:
	typedef ScriptingService *ServicePtr_t;

	static bool initialise(ScriptingService *self);

	static bool shutdown(ScriptingService *self);

	scheme* get_scheme() { return &tinyscm; };
	SDL_Thread *get_server() { return server_thread; };
	void load(const std::string& fileName);
private:
	static scheme tinyscm;
	static SDL_Thread *server_thread;
	
};

}

#endif

