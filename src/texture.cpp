
#include <glew.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>
#include <SDL_opengl.h>
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
		SDL_assert(TextureService::mCurrentRenderer.lock());
		image->surface();
		std::shared_ptr<SDL_Surface> image_surface(image->surface());
		texture_ptr = SDL_CreateTextureFromSurface(TextureService::mCurrentRenderer.lock().get(), image_surface.get());
		return;
	}

	void Texture::select()
	{
		SDL_assert(isValid());
		SDL_GL_BindTexture(texture_ptr, nullptr, nullptr);
		SDL_assert(glGetError() == GL_NO_ERROR);
		return;
	}

	void Texture::deselect()
	{
		SDL_GL_UnbindTexture(texture_ptr);
		SDL_assert(glGetError() == GL_NO_ERROR);
		return;
	}

	Texture::~Texture()
	{
		SDL_DestroyTexture(texture_ptr);
	}

}
