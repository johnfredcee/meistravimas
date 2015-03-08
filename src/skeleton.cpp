
#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_net.h>
#include <SDL_thread.h>
#include <SOIL.h>
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
#include "timer.h"
#include "tinyscm_if.h"
#include "panel.h"
#include "gui.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "blendish.h"
#include "oui.h"
#include "gui.h"


using namespace venk;

const int screen_width  = 640;
const int screen_height = 480;

SDL_sem* global_lock = nullptr;

GLuint gVBO = 0;
GLuint gIBO = 0;

void render_texture(SDL_Texture *tex, SDL_Renderer* ren, int x, int y) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

using bufferPair_t = std::pair< std::shared_ptr<Buffer>, std::shared_ptr<Buffer> >;

bufferPair_t init_buffers() {
	/*
	 * Data used to seed our vertex array and element array buffers:
	 */
	static const GLfloat testVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f
	};
	static const GLushort testElements[] = { 0, 1, 2, 3 };
	ServiceCheckout<BufferManagerService> buffers;
	auto vb = buffers->make_buffer(GL_ARRAY_BUFFER, (const void*) testVertices, GL_FLOAT, 4, 2, GL_STATIC_DRAW);
	auto ib = buffers->make_buffer(GL_ELEMENT_ARRAY_BUFFER, (const void*) testElements, GL_UNSIGNED_SHORT, 4, 1, GL_STATIC_DRAW);
	bufferPair_t result = make_pair(vb, ib);
	return result;
}

bufferPair_t init_cube() {
	// Create our vertex and index vectors
	std::vector<GLfloat>  vert_list;
	std::vector<GLushort> index_list;
	// The cube's vertices
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(-0.5f);
	vert_list.push_back(-0.5f);
	// The cube's faces (not represented by quads, but rather triangles)
	index_list.push_back(1);
	index_list.push_back(5);
	index_list.push_back(6); // Right Face Tri 1
	index_list.push_back(6);
	index_list.push_back(2);
	index_list.push_back(1); // Right Face Tri 2
	index_list.push_back(5);
	index_list.push_back(4);
	index_list.push_back(7); // Back Face Tri 1
	index_list.push_back(7);
	index_list.push_back(6);
	index_list.push_back(5); // Back Face Tri 2
	index_list.push_back(4);
	index_list.push_back(0);
	index_list.push_back(3); // Left Face Tri 1
	index_list.push_back(3);
	index_list.push_back(7);
	index_list.push_back(4); // Left Face Tri 2
	index_list.push_back(0);
	index_list.push_back(4);
	index_list.push_back(5); // Top Face Tri 1
	index_list.push_back(5);
	index_list.push_back(1);
	index_list.push_back(0); // Top Face Tri 2
	index_list.push_back(3);
	index_list.push_back(0);
	index_list.push_back(1); // Front Face Tri 1
	index_list.push_back(1);
	index_list.push_back(2);
	index_list.push_back(3); // Front Face Tri 2
	index_list.push_back(2);
	index_list.push_back(3);
	index_list.push_back(7); // Bottom Face Tri 1
	index_list.push_back(7);
	index_list.push_back(6);
	index_list.push_back(2); // Bottom Face Tri 2
	ServiceCheckout<BufferManagerService> buffers;
	auto vb = buffers->make_buffer(GL_ARRAY_BUFFER, (const void*) &vert_list[0], GL_FLOAT, vert_list.size()/3, 3, GL_STATIC_DRAW);
	auto ib = buffers->make_buffer(GL_ELEMENT_ARRAY_BUFFER, (const void*) &index_list[0], GL_UNSIGNED_SHORT, index_list.size(), 1, GL_STATIC_DRAW);
	bufferPair_t result = make_pair(vb, ib);
	return result;
}

void render(double alpha, SDL_Window* window, SDL_Renderer* renderer, Texture* texture, Program *simple, bufferPair_t buffers) {
	(void) renderer;
	(void) alpha;
	(void) texture;
	// perspective view - fairly generous 45.0f fov
	Matrix44 projection;
	projection.persp(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
	Vector3d eyePos(1.0f, 3.0f, 5.0f);
	Vector3d eyeTarget(0.0f, 0.0f, 0.0f);
	// look at the origin
	Matrix44 view;
	view.lookAt(eyePos, eyeTarget);
	//view.identity();
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	simple->use();
	// texture->select();
	// simple->setUniformSampler("textureMap", 0, texture);
	ColorRGBA red(1.0f, 0.0f, 0.0f, 0.0f);
	simple->setUniform4f("vColor", &red.elements[0]);
	simple->setUniformMat4f("mView", &view.elements[0]);
	simple->setUniformMat4f("mProj", &projection.elements[0]);
	auto vb = buffers.first;
	auto ib = buffers.second;
	vb->bindAttribute(simple, "vVertex");
	ib->bindIndices();
	ib->draw(GL_TRIANGLES);
	ib->unbindIndices();
	vb->unbindAttribute(simple, "vVertex");
	//texture->deselect();
	simple->unUse();
}

void update(double t, double dt) {
	(void) t;
	(void) dt;
}


int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	int physfsok = PHYSFS_init(argv[0]);
	if(!physfsok) {
		std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
	}
	if(SDLNet_Init() < 0) {
		std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
		return -1;
	}
	ServiceRegistry<ScriptingService>::initialise();
	if (argc > 1) {
		ServiceCheckout<ScriptingService> scripting;		
		for(int i = 0; i < argc-1; i++) {
			scripting->load(argv[i-1]);
		}
	}
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |  SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	} else {

		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		
		int physfsok = PHYSFS_setSaneConfig("yagc", "skeleton", "ZIP", 0, 0);
		if(!physfsok) {
			std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
		}
		std::cout << "Resource path is: " << getResourcePath() << std::endl;
		physfsok = PHYSFS_addToSearchPath(getResourcePath().c_str(), 1);
		if(!physfsok) {
			std::cerr << "PhysicsFS Init error" << PHYSFS_getLastError() << std::endl;
		}
		std::cout << "Dir listing.." << std::endl;
		char** flist = PHYSFS_enumerateFiles("images");
		for(char** i = flist; *i != nullptr; i++)
			std::cout << *i << std::endl;
		std::cout << "=============" << std::endl;
		PHYSFS_freeList(flist);

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
		} else {

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
			std::shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(window.get(), -1, flags),  renderer_deleter);
			if(renderer) {

				// we created renderer, get info about it
				SDL_RendererInfo info;
				SDL_GetRendererInfo(renderer.get(), &info);
				std::cout << " Renderer chosen " << info.name << std::endl;
				SDL_GLContext glctx = SDL_GL_CreateContext(window.get());
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

				// Now, we need keyboard and mouse
				ServiceRegistry<MouseService>::initialise();
				ServiceRegistry<KeyboardService>::initialise();
				
				//Use Vsync
				if(SDL_GL_SetSwapInterval(1) < 0) {
					std::cerr <<  "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
				}
				// Enable depth testing
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				// Set the clear color for when we re-draw the scene
				glClearColor(0.1, 0.1, 0.6, 1.0);
				glViewport(0, 0, screen_width, screen_height);
				SDL_assert(glGetError() == GL_NO_ERROR);
				ServiceRegistry<ImageService>::initialise();
				ServiceCheckout<ImageService> images;
				std::shared_ptr<Image> img(images->loadImage("test.tga"));
				if(img) {
					ServiceRegistry<TextureService>::initialise();
					ServiceRegistry<ProgramService>::initialise();
					ServiceRegistry<BufferManagerService>::initialise();
					ServiceRegistry<RenderStateService>::initialise();
					ServiceRegistry<GuiService>::initialise();
					{
						ServiceCheckout<TextureService> textures;
						textures->setRenderer(renderer);
						std::shared_ptr<Texture> tex(textures->makeTexture(img.get(), true));
						ServiceCheckout<ProgramService> programs;
						std::shared_ptr<Program> simple(programs->loadProgram("color"));
						SDL_assert(simple->isValid());
						// initialise the geometry
						TorusBuilder torus(1.0f, 0.25f, 16, 32);
						bufferPair_t buffers(torus.make_vertices(), torus.make_indices());
						// perspective view - fairly generous 45.0f fov
						// Matrix44 projection;
						// projection.persp(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
						// Vector3d eyePos(1.0f, 3.0f, 5.0f);
						// // look at the origin
						// Matrix44 view;
						// view.lookAt(eyePos, 0.0f, 0.0, 0.0f);
						// ServiceCheckout<RenderStateService> renderState;
						// // to do - clean this up with a template make_parameter fn
						// RenderParameter projParam = projection;
						// RenderParameter viewParam = view;
						// RenderParameter eye = eyePos;
						// renderState->set("projection", projParam);
						// renderState->set("view", viewParam);
						// renderState->set("eye", eye);
						if(tex) {
							double  t    = 0.0;
							double  dt   = 0.01;
							double currentTime = timeNow();
							double accumulator = dt;
							bool	quit = false;
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
								
								render(1.0 - alpha, window.get(), renderer.get(), tex.get(), simple.get(), buffers);
								{
									ServiceCheckout<GuiService> gui;
									gui->render();
								}
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
									switch(e.type)
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
											SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
											break;
									}
								}
																						
							}
							if (global_lock != nullptr) {
								SDL_DestroySemaphore(global_lock);
								global_lock = nullptr;
							}
						} else {
							std::cerr << "Creating texture failed" << std::endl;
						}
					}
					ServiceRegistry<GuiService>::shutdown();
					ServiceRegistry<RenderStateService>::shutdown();
					ServiceRegistry<BufferManagerService>::shutdown();
					ServiceRegistry<ProgramService>::shutdown();
					ServiceRegistry<TextureService>::shutdown();
					SDL_GL_DeleteContext(glctx);
				} else {
					std::cerr << "Loading image failed" << std::endl;
				}
			} else {
				std::cerr << "Creating renderer failed" << std::endl;
			}
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


