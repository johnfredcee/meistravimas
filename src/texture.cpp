

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_net.h>
#include <scheme-private.h>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "image.h"
#include "texture.h"

namespace venk
{

	std::weak_ptr<SDL_Renderer> TextureService::mCurrentRenderer = std::weak_ptr<SDL_Renderer>();

	bool TextureService::initialise(TextureService* self)
	{
		(void) self;
		return true;
	}

	bool TextureService::shutdown(TextureService* self)
	{
		(void) self;
		return true;
	}

	std::shared_ptr<Texture> TextureService::makeTexture(const char *name, const bool sdl)
	{
		ServiceCheckout<ImageService> images;
		std::shared_ptr<Image> image(images->getImage(name));
		return std::make_shared<Texture>(image.get(), sdl);
	}

	std::shared_ptr<Texture> TextureService::makeTexture(const Image *img, const bool sdl)
	{
		ServiceCheckout<ImageService> images;
		return std::make_shared<Texture>(img,sdl);
	}

	
	Texture::Texture(const Image* image, const bool sdl) : texture_ptr(nullptr)
	{
		SDL_assert(image != nullptr);
		SDL_assert(image->getData() != nullptr);
		SDL_assert(TextureService::mCurrentRenderer.lock());
		if (sdl) {
			makeSDLTexture(image);
		}
		glGenTextures(1, &gl_texture);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
					 image->width(), image->height(), 0, GL_RGBA, 
					 GL_UNSIGNED_BYTE, image->getData());
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}

	void Texture::makeSDLTexture(const Image* image)
	{
		image->surface();
		std::shared_ptr<SDL_Surface> image_surface(image->surface());
		texture_ptr = SDL_CreateTextureFromSurface(TextureService::mCurrentRenderer.lock().get(), image_surface.get());
	}
	
	SDL_Texture* Texture::getSDL_Texture()
	{
		SDL_assert(texture_ptr != nullptr);
		return texture_ptr;
	}
	
	void Texture::select(GLuint i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
	}
	
	void Texture::deselect(GLuint i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);		
	}
	
	void Texture::selectSDL_Texture()
	{
		SDL_assert(texture_ptr != nullptr);
		SDL_assert(isValid());
		SDL_GL_BindTexture(texture_ptr, nullptr, nullptr);
		SDL_assert(glGetError() == GL_NO_ERROR);
		return;
	}

	void Texture::deselectSDL_Texture()
	{
		SDL_assert(texture_ptr != nullptr);
		SDL_GL_UnbindTexture(texture_ptr);
		SDL_assert(glGetError() == GL_NO_ERROR);
		return;
	}

	Texture::~Texture()
	{
		SDL_DestroyTexture(texture_ptr);
	}

}
