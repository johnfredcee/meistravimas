

#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <array>
#include <string>
#include <functional>
#include <memory>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include <physfs.h>
#include <scheme-private.h>
#include <res_path.h>

#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "quat.h"
#include "colour.h"
#include "matrix44.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "serviceregistry.h"
#include "observer.h"
#include "mouse.h"
#include "keyboard.h"
#include "image.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "bufferbuilder.h"
#include "scripting.h"
#include "torus.h"
#include "renderstate.h"
#include "sprite.h"
#include "timer.h"
#include "tinymt32.h"
#include "random.h"
#include "tinyscm_if.h"
#include "picojson.h"
#include "jsonread.h"
#include "canvas.h"
#include "font.h"
#include "context.h"
#include "textedit_if.h"
#include "textedit.h"

using namespace std;
using namespace venk;

SDL_sem *global_lock = nullptr;
int screen_width = 960;
int screen_height = 540;
shared_ptr<SDL_Window> window;
shared_ptr<SDL_Renderer> renderer;
static SDL_GLContext context;

int main(int argc, char *argv[])
{

	(void)argc;

	// init SDL
	if (init_SDL() != 0)
	{
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (!initPhysfs("console", argv))
	{
		SDL_Quit();
	}
	SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.

	// create window
	shared_ptr<SDL_Window> window(create_window("console", screen_width, screen_height));
	if (!window)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to create SDL_Window");
		return 1;
	}

	// create render
	shared_ptr<SDL_Renderer> renderer = create_renderer(window.get());
	if (!renderer)
	{
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create SDL_Renderer: %s", SDL_GetError());
		return 1;
	}

	// setup OpenGL contezt
	context = opengl_setup(renderer.get(), window.get(), screen_width, screen_height);

	// Now, we need keyboard and mouse11
	ServiceRegistry<MouseService>::initialise();
	ServiceRegistry<KeyboardService>::initialise();

	ServiceRegistry<ScriptingService>::initialise();
	ServiceRegistry<ImageService>::initialise();
	ServiceRegistry<CanvasService>::initialise();
	ServiceRegistry<FontService>::initialise();
	ServiceRegistry<TextEditService>::initialise();
	{
		ServiceCheckout<CanvasService> canvas;
		ServiceCheckout<FontService> fonts;

		auto cozetteFont = fonts->createFont("cozette", "CozetteVector.ttf");
		fonts->setCurrentFont(cozetteFont);
		auto metrics = fonts->getFontMetrics();

		// Disable depth test and face culling.
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		int width, height;
		SDL_GetWindowSize(window.get(), &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(Colors::cornflowerblue(0), Colors::cornflowerblue(1), Colors::cornflowerblue(2), 0.0f);

		Uint32 currentTime = SDL_GetTicks();
		Uint32 lastTime = currentTime;

		float deltaTime;

		SDL_Event event;
		bool quit = false;
		while (!quit)
		{

			lastTime = currentTime;
			currentTime = SDL_GetTicks();
			deltaTime = (currentTime - lastTime) / 1000.0f;

			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					quit = true;
				}
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window.get()))
				{
					quit = true;
				}
			}

			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT);
			canvas->begin_frame(width, height);
			const char *message = "Hello World";
			fonts->drawText(20, 20, message, message + 12);
			canvas->end_frame();
			SDL_GL_SwapWindow(window.get());
		}
	}

	// Cleanup
	ServiceRegistry<TextEditService>::shutdown();
	ServiceRegistry<FontService>::shutdown();
	ServiceRegistry<CanvasService>::shutdown();
	ServiceRegistry<ImageService>::shutdown();
	ServiceRegistry<ScriptingService>::shutdown();

	SDL_GL_DeleteContext(context);

	// shutdown input
	ServiceRegistry<KeyboardService>::shutdown();
	ServiceRegistry<MouseService>::shutdown();

	shutdownPhysfs();
	SDL_Quit();
	return 0;
}
