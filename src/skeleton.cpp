
#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <array>
#include <string>
#include <functional>
#include <memory>
#include <GL/glew.h>
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
#include "panel.h"
#include "gui.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "blendish.h"
#include "oui.h"
#include "gui.h"


bool	quit = false;
SDL_sem* global_lock = nullptr;
int screen_width  = 640;
int screen_height = 480;

namespace venk {


std::vector< std::shared_ptr<Sprite> > spriteBank;

void render(double alpha, SDL_Window* window, SDL_Renderer* renderer) {
	(void) renderer;
	(void) alpha;
	(void) window;
	ServiceCheckout<SpriteService> sprites;
	sprites->render(alpha, window, renderer);
}

void update(double t, double dt) {
	(void) t;
	(void) dt;
}

void *malloc_shim(PHYSFS_uint64 size) {
	return SDL_malloc((size_t) size);
}

void *realloc_shim(void *mem, PHYSFS_uint64 size) {
	return SDL_realloc(mem, (size_t) size);
}

int initPhysfs(char **argv) {
	PHYSFS_Allocator sdlAllocator;
	sdlAllocator.Init = nullptr;
	sdlAllocator.Deinit = nullptr;
	sdlAllocator.Malloc = malloc_shim;
	sdlAllocator.Realloc = realloc_shim;
	sdlAllocator.Free = SDL_free;
	int physfsok = PHYSFS_init(argv[0]);
	if(!physfsok) {
		std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
		return physfsok;
	}
	PHYSFS_setAllocator(&sdlAllocator);
	physfsok = PHYSFS_setSaneConfig("yagc", "skeleton", "ZIP", 0, 0);
	if(!physfsok) {
		std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
		return physfsok;
	}
	std::cout << "Resource path is: " << getResourcePath() << std::endl;
	PHYSFS_mount(getResourcePath().c_str(), nullptr, 1);
	if(!physfsok) {
		std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
	}
	return physfsok;
}

std::shared_ptr<SDL_Window> create_window() {

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	// create window
	auto window_deleter = [](SDL_Window *w) {
		if(w) SDL_DestroyWindow(w);
	};
	std::shared_ptr<SDL_Window> window(SDL_CreateWindow("SDLSkeleton",
	                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                                   screen_width, screen_height,
	                                   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
	                                   window_deleter);
	if(!window) {
		std::cerr << "Unable to create SDL_Window" << std::endl;
	}
	return window;
}

std::shared_ptr<SDL_Renderer> create_renderer(SDL_Window* window) {
	// we created window, create renderer
	auto renderer_deleter = [](SDL_Renderer *r) {
		if(r) SDL_DestroyRenderer(r);
	};
	Uint32 n_drivers = SDL_GetNumRenderDrivers();
	std::cout << n_drivers << " render drivers available" << std::endl;
	for(Uint32 i = 0; i < n_drivers; i++) {
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
		std::cout << i << " " << info.name << std::endl;
	}
	Uint32 flags = SDL_RENDERER_ACCELERATED  |  SDL_RENDERER_TARGETTEXTURE;
	std::shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(window, -1, flags),  renderer_deleter);
	return renderer;
}

SDL_GLContext opengl_setup(SDL_Renderer* renderer, SDL_Window* window) {
	// we created renderer, get info about it
	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	std::cout << " Renderer chosen " << info.name << std::endl;
	SDL_GLContext glctx = SDL_GL_CreateContext(window);
	SDL_assert(glctx != nullptr);

	// init glew so we can do OpenGL properly
	Uint32 err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "Glew init failed " << glewGetErrorString(GLEW_VERSION) << std::endl;
		SDL_Quit();
		exit(-1);
	}
	GLdouble maj = 0;
	glGetDoublev(GL_MAJOR_VERSION, &maj);
	GLdouble min = 0;
	glGetDoublev(GL_MINOR_VERSION, &min);
	std::cout << "OpenGL Version: " << maj << "." << min << std::endl;
	//Use Vsync
	if(SDL_GL_SetSwapInterval(1) < 0) {
		std::cerr <<  "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
	}
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, screen_width, screen_height);
	SDL_assert(glGetError() == GL_NO_ERROR);
	return glctx;
}

void create_sprites(std::shared_ptr<Texture> sheet) {
	ServiceCheckout<SpriteService> sprites;
	RandomContext spriteRandom;
	for(int i = 0; i < 6; i++) {
		for(int j = 0; j < 6; j++) {
			std::shared_ptr<Sprite> sprite(sprites->createSprite(sheet, i, j, 16, 16));
			sprite->setXY(spriteRandom.nextRandom() * screen_width, spriteRandom.nextRandom() * screen_height);
			spriteBank.push_back(sprite);
		}
	}
	return;
}

void renderer_setup() {
	// does nothing now we are deprecating the render state service
}

std::shared_ptr<Texture> load_texture(const std::string& name) {
	ServiceCheckout<ImageService> images;
	ServiceCheckout<TextureService> textures;
	std::shared_ptr<Image> image(images->loadImage(name.c_str()));
	std::shared_ptr<Texture> texture(textures->makeTexture(image.get()));
	return texture;
}

} //namespace venk

using namespace venk;

int main(int argc, char **argv) {

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |  SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	} else {
		(void) argc;
		(void) argv;
		if(SDLNet_Init() < 0) {
			std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
			return -1;
		}
		initPhysfs(argv);
		ServiceRegistry<ScriptingService>::initialise();
		if (argc > 1) {
			ServiceCheckout<ScriptingService> scripting;
			for(int i = 0; i < argc-1; i++) {
				scripting->load(argv[i-1]);
			}
		}
		std::shared_ptr<SDL_Window> window(create_window());
		if (window) {
			std::shared_ptr<SDL_Renderer> renderer(create_renderer(window.get()));
			if(renderer) {
				SDL_GLContext glctx = opengl_setup(renderer.get(), window.get());

				// Now, we need keyboard and mouse
				ServiceRegistry<MouseService>::initialise();
				ServiceRegistry<KeyboardService>::initialise();
				ServiceRegistry<ImageService>::initialise();
				ServiceRegistry<TextureService>::initialise();

				std::shared_ptr<Texture> background(load_texture("test.tga"));
				std::shared_ptr<Texture> spritesheet(load_texture("boulders.png"));
				ServiceRegistry<ProgramService>::initialise();
				ServiceRegistry<BufferManagerService>::initialise();
				ServiceRegistry<RenderStateService>::initialise();
				ServiceRegistry<SpriteService>::initialise();
				renderer_setup();
				create_sprites(spritesheet);
#ifdef USE_GUI
				ServiceRegistry<GuiService>::initialise();
#endif
				{
					ServiceCheckout<ProgramService> programs;
					std::shared_ptr<Program> backdrop_shader(programs->loadProgram("backdrop"));
					SDL_assert(backdrop_shader->isValid());
					double  t    = 0.0;
					double  dt   = 0.01;
					double currentTime = timeNow();
					double accumulator = dt;
					global_lock = SDL_CreateSemaphore( 1 );
					SDL_Event e;
					while(!quit) {
						double newTime = timeNow();
						double frameTime = newTime - currentTime;
						if(frameTime > 0.25)
							frameTime = 0.25;
						currentTime = newTime;
						accumulator += frameTime;
						while(accumulator >= dt) {
							SDL_SemWait(global_lock);
							update(t, dt);
							SDL_SemPost(global_lock);
							t += dt;
							accumulator -= dt;
						}
						const double alpha = accumulator / dt;
						SDL_SemWait(global_lock);

						render(1.0 - alpha, window.get(), renderer.get());
#ifdef USE_GUI
						{
							ServiceCheckout<GuiService> gui;main
							gui->render();
						}
#endif
						SDL_GL_SwapWindow(window.get());

						SDL_SemPost(global_lock);

						//Handle events on queue
						SDL_PumpEvents();
						while(SDL_PeepEvents(&e,1,SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) != 0) {
							//User requests quit
							if(e.type == SDL_QUIT) {
								quit = true;
							}
							// theoretically this code could be replaced by an event filter (TODO)
							switch(e.type) {
							case SDL_MOUSEWHEEL:
							case SDL_MOUSEMOTION:
							case SDL_MOUSEBUTTONUP:
							case SDL_MOUSEBUTTONDOWN: {
								SDL_SemWait(global_lock);
								ServiceCheckout<MouseService> squeak;
								squeak->mouse();
								SDL_SemPost(global_lock);

							}
							break;
							case SDL_KEYUP:
							case SDL_KEYDOWN: {
								SDL_SemWait(global_lock);
								ServiceCheckout<KeyboardService> keyb;
								keyb->keyboard();
								SDL_SemPost(global_lock);

							}
							break;
							default:
								SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
								break;
							}
						}
					}
					// quit main loop
					if (global_lock != nullptr) {
						SDL_DestroySemaphore(global_lock);
						global_lock = nullptr;
					}
				}
#ifdef USE_GUI
				ServiceRegistry<GuiService>::shutdown();
#endif
				spriteBank.erase(spriteBank.begin(), spriteBank.end());
				ServiceRegistry<SpriteService>::shutdown();
				ServiceRegistry<RenderStateService>::shutdown();
				ServiceRegistry<BufferManagerService>::shutdown();
				ServiceRegistry<ProgramService>::shutdown();
				ServiceRegistry<TextureService>::shutdown();
				SDL_GL_DeleteContext(glctx);

				ServiceRegistry<ImageService>::shutdown();
				ServiceRegistry<KeyboardService>::shutdown();
				ServiceRegistry<MouseService>::shutdown();
			}
		}
		ServiceRegistry<ScriptingService>::shutdown();
		SDLNet_Quit();
		int phsyfhshutdown = PHYSFS_deinit();
		if(!phsyfhshutdown) {
			std::cerr << "Physfs shutdown failed:" << PHYSFS_getLastError() << std::endl;
		}
		SDL_Quit();
		return 0;
	}
}
