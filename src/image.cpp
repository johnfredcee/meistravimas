
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <cctype>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_net.h>
#include <SOIL.h>
#include <res_path.h>
#include <scheme-private.h>

#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "image.h"



namespace venk
{

const char *ImageService::computeName(const char *name)
{
    static char imageName[Image::maxImageNameLength];

    extern size_t strnlen(const char *string, size_t maxlen);

    // skip '.tga' 
    int end = 0;
    while (isalnum(name[end]))
        end++;
    strncpy(imageName, name, end);
    imageName[end] = '\0';
    return imageName;
}

bool ImageService::initialise(ImageService* self)
{
	(void) self;
    return true;
}

bool ImageService::shutdown(ImageService* self)
{
	self->imageTable.clear();
    return true;
}

std::shared_ptr<Image> ImageService::loadImage(const char *filename)
{
	SDL_assert((filename != nullptr) && (strlen(filename) < Image::maxImageNameLength));
    std::shared_ptr<Image> image = std::make_shared<Image>(filename);
    const char *imagename = computeName(filename);
    std::cerr << "Loaded image " << imagename << std::endl;
    imageTable.insert(ImageLookupTable_t::value_type(imagename, image));
    return image;
}

std::shared_ptr<Image> ImageService::getImage(const char *name)
{
    const char *imagename = computeName(name);
    return std::shared_ptr<Image>(imageTable.find(imagename)->second);
}

Image::Image(const std::string& fileName) : mPixels(nullptr)
{
    std::string fullFileName = getResourcePath();
    fullFileName = fullFileName + fileName;
    SDL_RWops *rwops = SDL_RWFromFile(fullFileName.c_str(), "rb");
    if (rwops != nullptr)
    {
        Sint64 size = SDL_RWsize(rwops);
        if (size != -1L)
        {
            void *buf = SDL_malloc(size);
            size_t read = SDL_RWread(rwops, buf, 1, size);
            if (read == size)
            {
                mPixels = SOIL_load_image_from_memory((const unsigned char *)buf, size, &mWidth, &mHeight, &mChannels, SOIL_LOAD_AUTO);
                if (mPixels != nullptr)
                {
                    SDL_RWclose(rwops);
                    return;
                }
                else
                {
                    std::cerr << "Creating " << fileName << " failed" << std::endl;
                }
            }
            else
            {
                std::cerr << "Reading " << fileName << " failed" << std::endl;
            }
            SDL_free(buf);
        }
        else
        {
            std::cerr << "Probing " << fileName << " failed" << std::endl;
        }
    }
    else
    {
        std::cerr << "Opening " << fileName << " failed" << std::endl;
    }
    SDL_RWclose(rwops);
    return;
}

std::shared_ptr<SDL_Surface> Image::surface() const
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif

    auto surface_deleter = [](SDL_Surface * s)
    {
        if (s) SDL_FreeSurface(s);
    };
    std::shared_ptr<SDL_Surface> result(SDL_CreateRGBSurfaceFrom(mPixels, mWidth, mHeight, mChannels * 8, mWidth * mChannels, rmask, gmask, bmask, amask ),
                                        surface_deleter);
    return result;
}

Image::~Image()
{
    if (mPixels != nullptr)
        SOIL_free_image_data(mPixels);
}

// Scheme bindings

extern "C"
{
	pointer add_image_from_scheme(scheme *sc, pointer args)
	{
		pointer arg;

		if( is_string( arg = pair_car(args)) )
		{
			char *name;
			name = string_value( arg );
			std::cout << "Creating image named " << name << std::endl;
		} else {
			return sc->F;
		}		
		return sc->T;
	}
}

}


