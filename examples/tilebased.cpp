

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

#define IMGUI_IMPL_OPENGL_LOADER_GLAD 1
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

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

using namespace std;
using namespace venk;

SDL_sem* global_lock = nullptr;
int screen_width = 960;
int screen_height = 540;
shared_ptr<SDL_Window> window;
shared_ptr<SDL_Renderer> renderer;
static SDL_GLContext context;
static const char *title = "OpenGL 3.3";

shared_ptr<SDL_Window> create_window(const char *caption)
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
	shared_ptr<SDL_Window> window(SDL_CreateWindow(caption,
												   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
												   screen_width, screen_height,
												   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
								  window_deleter);
	if (!window)
	{
	 	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to create SDL_Window" );
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
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,  "%d render drivers available", n_drivers);
	for (Uint32 i = 0; i < n_drivers; i++)
	{
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%d %s", i, info.name);
	}
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(window, -1, flags), renderer_deleter);
	if (!renderer)
	{
		std::cerr << "Unable to create SDL_Renderer" << endl;
		SDL_Quit();
	}
	return renderer;
}

SDL_GLContext opengl_setup(SDL_Renderer *renderer, SDL_Window *window)
{
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	std::cout << "OpenGL loading" << std::endl;
	int result = gladLoadGLLoader(SDL_GL_GetProcAddress);
	if (result == 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "OpenGL loader failed");
		SDL_Quit();
	}
	SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Vendor:   %s\n", glGetString(GL_VENDOR));
    SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Renderer: %s\n", glGetString(GL_RENDERER));
    SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Version:  %s\n", glGetString(GL_VERSION));	
	SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "GLSL Version: %s.", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Use v-sync
	SDL_GL_SetSwapInterval(1);
	SDL_assert(context != nullptr);
	SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	return context;
}

void imgui_setup(SDL_Window *window, SDL_GLContext context)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char *glsl_version = "#version 330";

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void *malloc_shim(PHYSFS_uint64 size)
{
	return SDL_malloc((size_t)size);
}

void *realloc_shim(void *mem, PHYSFS_uint64 size)
{
	return SDL_realloc(mem, (size_t)size);
}

// std::string getResourcePath()
// {
// 	char* base_path = SDL_GetBasePath();
//     std::string  fullPath = base_path;
// 	SDL_free(base_path);
// 	return fullPath;
// }

int initPhysfs(char **argv)
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
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode() ) );
		return physfsok;
	}
	PHYSFS_setAllocator(&sdlAllocator);
	physfsok = PHYSFS_setSaneConfig("yagc", "skeleton", "ZIP", 0, 0);
	if (!physfsok)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode() ) );
		return physfsok;
	}
	SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Resource Path is %s", getResourcePath().c_str() );
	PHYSFS_mount(getResourcePath().c_str(), nullptr, 1);
	if (!physfsok)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "PhysicsFS Init error %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode() ) );
	}
	return physfsok;
}

int main(int argc, char *argv[])
{

	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0)
	{
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (!initPhysfs(argv))
	{
		SDL_Quit();
	}
	SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.

	// create window
	shared_ptr<SDL_Window> window(create_window(title));
	if (!window)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unable to create SDL_Window" );
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
	context = opengl_setup(renderer.get(), window.get());

	imgui_setup(window.get(), context);
	// Now, we need keyboard and mouse
	ServiceRegistry<MouseService>::initialise();
	ServiceRegistry<KeyboardService>::initialise();
	ServiceRegistry<ImageService>::initialise();

	// Disable depth test and face culling.
	// glDisable(GL_DEPTH_TEST);
	// glDisable(GL_CULL_FACE);

	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	//int width, height;
	// SDL_GetWindowSize(window.get(), &width, &height);
	// glViewport(0, 0, width, height);
	// glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Uint32 currentTime = SDL_GetTicks();
	Uint32 lastTime = currentTime;

	float deltaTime;

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	{
		ServiceCheckout<ImageService> images;
		images->loadImage("ld-items-0.png");
		images->loadImage("ld-items-1.png");
		images->loadImage("ld-terrain-0.png");
		images->loadImage("ld-terrain-1.png");
		picojson::value items0 = read_json("ld-items-0.json");
		picojson::value items1 = read_json("ld-items-1.json");
		picojson::value terrain0 = read_json("ld-terrain-0.json");
		picojson::value terrain1 = read_json("ld-terrain-1.json");
	}
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

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window.get());
		ImGui::NewFrame();
		ImGui::Begin("Hello World");
		ImGui::End();
		ImGui::Render();

		//     SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
		//     SDL_RenderClear(renderer.get());

		//     SDL_SetRenderDrawColor(renderer.get(), 255, 0, 0, 255);
		//     SDL_Rect rect;
		//     rect.x = 100;
		//     rect.y = 100;
		//     rect.w = 100;
		//     rect.h = 100;
		//     SDL_RenderDrawRect(renderer.get(), &rect);
		//     SDL_RenderPresent(renderer.get());
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window.get());
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	ServiceRegistry<ImageService>::shutdown();

	SDL_GL_DeleteContext(context);

	// shutdown input
	ServiceRegistry<KeyboardService>::shutdown();
	ServiceRegistry<MouseService>::shutdown();

	int phsyfhshutdown = PHYSFS_deinit();
	if (!phsyfhshutdown)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Physfs shutdown failed:  %s ", PHYSFS_getErrorByCode( PHYSFS_getLastErrorCode() ) );
	}
	SDL_Quit();
	return 0;
}