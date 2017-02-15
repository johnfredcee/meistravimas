

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <glad/glad.h>
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

	std::shared_ptr<Texture> TextureService::makeTexture(const char *name)
	{
		ServiceCheckout<ImageService> images;
		std::shared_ptr<Image> image(images->getImage(name));
		return std::make_shared<Texture>(image.get());
	}

	std::shared_ptr<Texture> TextureService::makeTexture(const Image *img)
	{
		ServiceCheckout<ImageService> images;
		return std::make_shared<Texture>(img);
	}

	
	Texture::Texture(const Image* image) : texture_ptr(nullptr)
	{
		SDL_assert(image != nullptr);
		SDL_assert(image->getData() != nullptr);
		glGenTextures(1, &gl_texture);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
					 image->width(), image->height(), 0, GL_RGBA, 
					 GL_UNSIGNED_BYTE, image->getData());
		glBindTexture(GL_TEXTURE_2D, 0);
		width=image->width();
		height=image->height();
		alpha=image->hasAlpha();
		return;
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
	

	Texture::~Texture()
	{
		SDL_DestroyTexture(texture_ptr);
	}

}
