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
#include "panel.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "blendish.h"
#include "oui.h"
#include "gui.h"
#include "context.h"

#define USE_GUI 1

bool quit = false;
SDL_sem *global_lock = nullptr;
int screen_width = 640;
int screen_height = 480;

namespace venk
{
	std::vector<std::shared_ptr<Sprite>> spriteBank;

	void render(double alpha, SDL_Window *window, SDL_Renderer *renderer)
	{
		(void)renderer;
		(void)alpha;
		(void)window;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		ServiceCheckout<SpriteService> sprites;
		sprites->beginBatchWalk();
		std::shared_ptr<SpriteBatch> batch = sprites->nextBatch();
		while (batch)
		{
			batch->render(alpha, window, renderer);
			batch = sprites->nextBatch();
		}
	}

	void update(double t, double dt)
	{
		(void)t;
		(void)dt;
		ServiceCheckout<SpriteService> sprites;
	}

	void create_background(std::shared_ptr<Texture> background)
	{
		ServiceCheckout<SpriteService> sprites;
		RandomContext spriteRandom;
		std::weak_ptr<SpriteBatch> batch(sprites->addBatch(1, background));
		std::weak_ptr<Sprite> sprite(batch.lock()->addSprite(100.0f, 100.0f, 255.0f, 255.0f, 0, 0));
		return;
	}

	std::weak_ptr<SpriteBatch> create_sprites(std::shared_ptr<Texture> sheet)
	{
		ServiceCheckout<SpriteService> sprites;
		RandomContext spriteRandom;
		std::weak_ptr<SpriteBatch> batch(sprites->addBatch(6 * 6, sheet));
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				float u = (float)i * 16.0f;
				float v = (float)j * 16.0f;
				std::weak_ptr<Sprite> sprite(batch.lock()->addSprite(0.0f, 0.0f, 16.0f, 16.0f, u, v));
				sprite.lock()->setXY(spriteRandom.nextRandom() * screen_width, spriteRandom.nextRandom() * screen_height);
			}
		}
		return batch;
	}

	void renderer_setup()
	{
		// does nothing now we are deprecating the render state service
	}

	std::shared_ptr<Texture> load_texture(const std::string &name)
	{
		ServiceCheckout<ImageService> images;
		ServiceCheckout<TextureService> textures;
		std::shared_ptr<Image> image(images->loadImage(name.c_str()));
		std::shared_ptr<Texture> texture(textures->makeTexture(image.get()));
		return texture;
	}

} //namespace venk

using namespace venk;

void update_and_render(SDL_Window *window, SDL_Renderer *renderer,
					   double &currentTime, double &accumulator, double &dt, double &t)
{

	double newTime = timeNow();
	double frameTime = newTime - currentTime;
	if (frameTime > 0.25)
		frameTime = 0.25;
	currentTime = newTime;
	accumulator += frameTime;
	while (accumulator >= dt)
	{
		SDL_SemWait(global_lock);
		// update
		update(t, dt);
		SDL_SemPost(global_lock);
		t += dt;
		accumulator -= dt;
	}
	const double alpha = accumulator / dt;
	SDL_SemWait(global_lock);
	// render
	render(1.0 - alpha, window, renderer);
#ifdef USE_GUI
	{
		ServiceCheckout<GuiService> gui;
		gui->render();
	}
#endif
	SDL_GL_SwapWindow(window);
	SDL_SemPost(global_lock);
}

int main(int argc, char **argv)
{
	(void)argc;

	if (init_SDL() != 0)
	{
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	};
	initPhysfs("Skeleton", argv);
	ServiceRegistry<ScriptingService>::initialise();
	if (argc > 1)
	{
		ServiceCheckout<ScriptingService> scripting;
		for (int i = 0; i < argc - 1; i++)
		{
			scripting->load(argv[i - 1]);
		}
	}
	std::shared_ptr<SDL_Window> window(create_window("SDL Skeleton", screen_width, screen_height));
	if (window)
	{
		std::shared_ptr<SDL_Renderer> renderer(create_renderer(window.get()));
		if (renderer)
		{
			SDL_GLContext glctx = opengl_setup(renderer.get(), window.get(), screen_width, screen_height);

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
			//create_background(background);
			create_sprites(spritesheet);
#ifdef USE_GUI
			ServiceRegistry<GuiService>::initialise();
			{
				ServiceCheckout<GuiService> gui;
				int width, height;
				SDL_GetWindowSize(window.get(), &width, &height);
				gui->setCanvasDimensions(width, height);
			}
#endif
			{
				ServiceCheckout<ProgramService> programs;
				std::shared_ptr<Program> backdrop_shader(programs->loadProgram("backdrop"));
				SDL_assert(backdrop_shader->isValid());
				double t = 0.0;
				double dt = 0.01;
				double currentTime = timeNow();
				double accumulator = dt;
				global_lock = SDL_CreateSemaphore(1);
				SDL_Event e;
				while (!quit)
				{
					update_and_render(window.get(), renderer.get(),
									  currentTime, accumulator, dt, t);

					//Handle events on queue
					SDL_PumpEvents();
					while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) != 0)
					{
						//User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}
						// theoretically this code could be replaced by an event filter (TODO)
						switch (e.type)
						{
						case SDL_MOUSEWHEEL:
						case SDL_MOUSEMOTION:
						case SDL_MOUSEBUTTONUP:
						case SDL_MOUSEBUTTONDOWN:
						{
							SDL_SemWait(global_lock);
							ServiceCheckout<MouseService> squeak;
							squeak->mouse();
							SDL_SemPost(global_lock);
						}
						break;
						case SDL_KEYUP:
						case SDL_KEYDOWN:
						{
							SDL_SemWait(global_lock);
							ServiceCheckout<KeyboardService> keyb;
							keyb->keyboard();
							SDL_SemPost(global_lock);
						}
						break;
						default:
							break;
						}
					}
				}
				// quit main loop
				if (global_lock != nullptr)
				{
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
	shutdownPhysfs();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

