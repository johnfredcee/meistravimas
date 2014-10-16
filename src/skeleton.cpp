
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <memory>
#include <glew.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SOIL.h>

#include "tuple.h"
#include "threedee.h"
#include "matrix44.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "serviceregistry.h"
#include "image.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "renderstate.h"
#include <res_path.h>

using namespace venk;

const int screen_width  = 640;
const int screen_height = 480;

GLuint gVBO = 0; 
GLuint gIBO = 0;

void render_texture(SDL_Texture *tex, SDL_Renderer* ren, int x, int y)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void render(SDL_Window* window, Program *simple)
{
	ServiceCheckout<ProgramService> programs;

	//SDL_RenderClear(renderer.get());
	
	SDL_assert(simple->isValid());
	simple->use();
	
	
	
	//SDL_RenderCopy(renderer.get(), tex->getSDL_Texture(), nullptr, nullptr);
	//						SDL_GL_SwapWindow(window.get());
	// SDL_RenderPresent(renderer.get());
	SDL_GL_SwapWindow( window );
	simple->unUse();
	SDL_Delay(5000);

}

int main(int argc, char **argv)
{

	(void) argc;
	(void) argv;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
    } else {
	
		SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengl" );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE ); 


		std::cout << "Resource path is: " << getResourcePath() << std::endl;

		auto window_deleter = [] (SDL_Window *w) {
			if (w) SDL_DestroyWindow(w);
		};
		std::shared_ptr<SDL_Window> window(SDL_CreateWindow( "SDLSkeleton",
															 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
															 screen_width, screen_height,
															 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL),
										   window_deleter );
		if (!window) {
			std::cerr << "Unable to create SDL_Window" << std::endl;
		} else {
			auto renderer_deleter = [](SDL_Renderer *r) {
				if (r) SDL_DestroyRenderer(r);
			};
			Uint32 n_drivers = SDL_GetNumRenderDrivers();
			std::cout << n_drivers << " render drivers available" << std::endl;
			for(Uint32 i = 0; i < n_drivers; i++) {
				SDL_RendererInfo info;
				SDL_GetRenderDriverInfo(i, &info);
				std::cout << i << " " << info.name << std::endl;
			}
			Uint32 flags = SDL_RENDERER_ACCELERATED  |  SDL_RENDERER_TARGETTEXTURE;
			std::shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(window.get(), -1, flags),  renderer_deleter);
			SDL_GLContext glctx = SDL_GL_CreateContext(window.get());
			SDL_assert(glctx != nullptr);
			if (renderer) {
				
				Uint32 err = glewInit();
				if (err != GLEW_OK) {
					std::cerr << "Glew init failed " << glewGetErrorString(GLEW_VERSION) << std::endl;
					SDL_Quit();
					exit(-1);
				}

				//Use Vsync
				if( SDL_GL_SetSwapInterval( 1 ) < 0 ) { 
					std::cerr <<  "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
				}
		
				SDL_RendererInfo info;
				SDL_GetRendererInfo(renderer.get(), &info);
				std::cout << " Renderer chosen " << info.name << std::endl;

				ServiceRegistry<ImageService>::initialise();
				ServiceCheckout<ImageService> images;
				std::shared_ptr<Image> img(images->loadImage("test.tga"));
				if (img) {
					ServiceRegistry<TextureService>::initialise();
					ServiceCheckout<TextureService> textures;
					textures->setRenderer(renderer);
					std::shared_ptr<Texture> tex(textures->makeTexture(img.get()));
					ServiceRegistry<ProgramService>::initialise();
					ServiceCheckout<ProgramService> programs;
					std::shared_ptr<Program> simple(programs->loadProgram("simple"));
					SDL_assert(simple->isValid());			
					// ortho view, 100 units deep
					Matrix44 projection;
					projection.ortho(0.0f, (float) screen_width, 0.0f, (float) screen_height, 0.0f,  100.0f);
					ServiceRegistry<RenderStateService>::initialise();
					ServiceCheckout<RenderStateService> renderState;
					// to do - clean this up with a template make_parameter fn
					RenderParameter projParam;
					projParam.type = eMAT4;
					for(Uint32 i = 0; i < 16; i++)
						projParam.value.mat4[i] = projection.elements[i];
					renderState->set("projection", projParam);
					if (tex) {
						render(window.get(), simple.get());
					} else {
						std::cerr << "Creating texture failed" << std::endl;
					}
				} else {
					std::cerr << "Loading image failed" << std::endl;
				}
				//SDL_GL_DeleteContext(glctx);
			} else {
				std::cerr << "Creating renderer failed" << std::endl;
			}
		}
    }
    SDL_Quit();
    return 0;
}
