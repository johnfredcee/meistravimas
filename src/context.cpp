#include <memory>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include <physfs.h>
#include <res_path.h>

#include "context.h"

namespace venk
{
	void *malloc_shim(PHYSFS_uint64 size)
	{
		return SDL_malloc((size_t)size);
	}

	void *realloc_shim(void *mem, PHYSFS_uint64 size)
	{
		return SDL_realloc(mem, (size_t)size);
	}

	int init_SDL()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0)
		{
			return -1;
		}
		else
		{
			if (SDLNet_Init() < 0)
			{
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDLNet_Init: %s ", SDLNet_GetError());
				return -1;
			}
		}
		return 0;
	}

	int initPhysfs(const char* name, char **argv)
	{
		PHYSFS_Allocator sdlAllocator;
		sdlAllocator.Init = nullptr;
		sdlAllocator.Deinit = nullptr;
		sdlAllocator.Malloc = malloc_shim;
		sdlAllocator.Realloc = realloc_shim;
		sdlAllocator.Free = SDL_free;
		int physfsok = PHYSFS_init(argv[0]);
		if (!physfsok)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return physfsok;
		}
		PHYSFS_setAllocator(&sdlAllocator);
		physfsok = PHYSFS_setSaneConfig("yagc", name, "ZIP", 0, 0);
		if (!physfsok)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return physfsok;
		}
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Resource path is: %s",  getResourcePath().c_str());
		PHYSFS_mount(getResourcePath().c_str(), nullptr, 1);
		if (!physfsok)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
		return physfsok;
	}

	void shutdownPhysfs()
	{
		int phsyfhshutdown = PHYSFS_deinit();
		if (!phsyfhshutdown)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Physfs shutdown failed:  %s ", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}

	std::shared_ptr<SDL_Window> create_window(const char* caption, int width, int height)
	{

		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


		// create window
		auto window_deleter = [](SDL_Window *w) {
			if (w)
				SDL_DestroyWindow(w);
		};
		std::shared_ptr<SDL_Window> window(SDL_CreateWindow(caption != nullptr ? caption : "SDLApp",
															SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
															width, height,
															SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
										   window_deleter);
		if (!window)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to create SDL_Window");
			SDL_Quit();
		}
		return window;
	}

	std::shared_ptr<SDL_Renderer> create_renderer(SDL_Window *window)
	{
		// we created window, create renderer
		auto renderer_deleter = [](SDL_Renderer *r) {
			if (r)
				SDL_DestroyRenderer(r);
		};
		Uint32 n_drivers = SDL_GetNumRenderDrivers();
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%d render drivers available", n_drivers);
		for (Uint32 i = 0; i < n_drivers; i++)
		{
			SDL_RendererInfo info;
			SDL_GetRenderDriverInfo(i, &info);
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%d %s", i, info.name);
		}
		Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		std::shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(window, -1, flags), renderer_deleter);
		return renderer;
	}

	SDL_GLContext opengl_setup(SDL_Renderer *renderer, SDL_Window *window, int width, int height)
	{
		// we created renderer, get info about it
		SDL_RendererInfo info;
		SDL_GetRendererInfo(renderer, &info);
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Renderer chosen %s ", info.name);
		SDL_GLContext glctx = SDL_GL_CreateContext(window);
		if (glctx == nullptr)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Failed to create GL Context");
			SDL_Quit();
		}
		SDL_GL_MakeCurrent(window, glctx);

		int version = gladLoadGLLoader(SDL_GL_GetProcAddress);
		if (version == 0)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Glad failed to load gl");
			SDL_Quit();
		}
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Vendor:   %s\n", glGetString(GL_VENDOR));
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Renderer: %s\n", glGetString(GL_RENDERER));
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Version:  %s\n", glGetString(GL_VERSION));
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "GLSL Version: %s.", glGetString(GL_SHADING_LANGUAGE_VERSION));


		GLdouble maj = 0;
		glGetDoublev(GL_MAJOR_VERSION, &maj);
		GLdouble min = 0;
		glGetDoublev(GL_MINOR_VERSION, &min);
		SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "OpenGL Version: %1d.%1d", (int)maj, (int)min);

		//Use Vsync
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Warning: Unable to set VSync! SDL Error: %s. ", SDL_GetError());
		}
		SDL_RenderSetLogicalSize(renderer, width, height);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_assert(glGetError() == GL_NO_ERROR);
		return glctx;
	}

} // namespace venk
