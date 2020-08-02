
#ifndef CONTEXT_H
#define CONTEXT_H

namespace venk
{
	int init_SDL();
	int initPhysfs(const char* name, char **argv);
	void shutdownPhysfs();

	SDL_GLContext opengl_setup(SDL_Renderer* renderer, SDL_Window* window, int width, int height);
	std::shared_ptr<SDL_Renderer> create_renderer(SDL_Window* window);
	std::shared_ptr<SDL_Window> create_window(const char* title, int width, int height);


}

#endif