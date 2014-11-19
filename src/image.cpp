
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
#include <scheme-defs.h>
#include <scheme-private.h>
#include <scheme.h>

#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "scripting.h"
#include "tinyscm_if.h"
#include "image.h"

extern SDL_sem* global_lock;

extern "C" {
	void register_image_functions(scheme* sc);
}

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
	ServiceCheckout<ScriptingService> scripting;
	scheme* sc = scripting->get_scheme();
	register_image_functions(sc);
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

std::shared_ptr<Image> ImageService::makeImage(const char* name, const Uint8* mem, int width, int height, int channels)
{
    std::shared_ptr<Image> image = std::make_shared<Image>(mem, height, width, channels);
    imageTable.insert(ImageLookupTable_t::value_type(name, image));
    return image;
}

std::shared_ptr<Image> ImageService::getImage(const char *name)
{
    const char *imagename = computeName(name);
    return std::shared_ptr<Image>(imageTable.find(imagename)->second);
}

std::function<const char*()> ImageService::enumerateImages()
{
	ImageLookupTable_t::iterator  image_it = imageTable.begin();
	auto result = [=]() mutable -> const char*
	{
		if (image_it != imageTable.end()) {
			const char* name = (image_it->first).c_str();
			++image_it;				
			return name;
		}
		return nullptr;
	};
	return result;
}
// -------------------- Image methods --------------------

Image::Image(const Uint8* mem, int width, int height, int channels) : mWidth(width), mHeight(height), mChannels(channels)
{
	Sint64 size = width*height;
	mPixels = SOIL_load_image_from_memory((const unsigned char *)mem, size, &mWidth, &mHeight, &mChannels, SOIL_LOAD_AUTO);
	return;
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
    std::shared_ptr<SDL_Surface> result(
		SDL_CreateRGBSurfaceFrom(mPixels, mWidth, mHeight, mChannels * 8, mWidth * mChannels,
								 rmask, gmask, bmask, amask ),
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
	static const char* imageTag = "IMAGE";

	/* to manage the lifetime of images in scheme */
	typedef std::unordered_map< std::string, std::shared_ptr<Image> > SchemeImageTable_t;
	SchemeImageTable_t scheme_images;

	/* this will get called when the scheme garbage collector wants it gone */
	void scheme_image_delete(void* ptr)
	{
		const char *ref = (const char*) ptr;
		scheme_images.erase(ref);
		SDL_free(ptr);
	}

	/* add an image in scheme vector form to the image management */
	pointer add_image_from_scheme(scheme *sc, pointer args)
	{
		hold_global_lock();
		pointer name_arg;
		if( is_string( name_arg = pair_car(args)) )
		{
			char *name;
			name = string_value( name_arg );
			std::cout << "Creating image named " << name << std::endl;
			if (scheme_images.find(name) != scheme_images.end() ) 
			{
				std::cout << "Image named " << name << " already exists " << std::endl;
				goto bad_args;
			}
			// to do - need height & width
			args = pair_cdr(args);
			long width = 0;
			if ((args != nullptr) && (is_integer(pair_car(args))))
			{
				width = ivalue(pair_car(args));
			} else {
				goto bad_args;
			}
			// to do - need height & width
			args = pair_cdr(args);
			long height = 0;
			if ((args != nullptr) && (is_integer(pair_car(args))))
			{
				height = ivalue(pair_car(args));
			} else {
				goto bad_args;
			}					
			args = pair_cdr(args);
			if (!is_pair(args))
				goto bad_args;
			pointer image_arg = pair_car(args);
			if ( is_pair(image_arg) ) {
				int    channel_size = height * width;
				int    nchannels = list_length(sc, image_arg);				
				int    channeli  = nchannels - 1;
				Uint8 *buf = (Uint8*) SDL_malloc(nchannels * channel_size);
				while ((image_arg != nullptr) && (image_arg != sc->NIL))
				{	
					if (!is_pair(image_arg))
						goto bad_args;
					
					if (!is_pair(pair_car(image_arg)))
						goto bad_args;
					
					pointer channel_name = pair_car(pair_car(image_arg));
					if (is_symbol(channel_name)) {
						std::cout << "Channel : " << symname(channel_name) << std::endl;
					} else {
						goto bad_args;
					}
					pointer channel_data = pair_car(pair_cdr(image_arg));
					if (is_vector(channel_data)) {
						channel_size = ivalue(channel_data);
						std::cout << "Size : " << ivalue(channel_data) << " bytes." << std::endl;
						for(Sint32 i = 0; i < ivalue(channel_data); i++) {
							pointer element = vector_elem(channel_data,i);
							if (!is_number(element))
								goto bad_args;
							buf[i*nchannels+channeli] = (Uint8)(rvalue(element) * 255.0);
						}
					} else {
						goto bad_args;
					}
					channeli--;
					image_arg = pair_cdr(image_arg);
				}
				ServiceCheckout<ImageService> images;
				// to do -- table of scheme managed images / table or opaque type?
				std::shared_ptr<Image> scheme_image = images->makeImage(name, buf, width, height, nchannels);
				SDL_free(buf);			
				auto result = scheme_images.emplace(make_pair(name, scheme_image));
				if (!result.second) {
					std::cout << "image : " << name << " insertion failed" << std::endl;
					goto bad_args;
				}
				const char* ref = SDL_strdup(name);
				release_global_lock();
				return mk_opaque(sc, imageTag, (void*) ref, scheme_image_delete);
			}
		} 
  bad_args:
		release_global_lock();
		std::cout << "Bad arguments " << std::endl;
		return sc->F;
	}

	pointer image_names(scheme* sc, pointer args)
	{
		hold_global_lock();
		if(args != sc->NIL)
		{
			return sc->F;
		}		
		ServiceCheckout<ImageService> images;
		auto enumerator = images->enumerateImages();
		const char* name = nullptr;
		pointer result = sc->NIL;
		while( (name = enumerator()) != nullptr )
		{
			result = cons(sc, mk_string(sc, name), result);
		}
		release_global_lock();
		return result;
	}

	void register_image_functions(scheme* sc)
	{
		/* inline lustration how to define a "foreign" function
		   implemented in C */
		scheme_define(sc,sc->global_env,mk_symbol(sc,"image-names"),mk_foreign_func(sc, image_names));
		scheme_define(sc,sc->global_env,mk_symbol(sc,"add-image"),mk_foreign_func(sc, add_image_from_scheme));
		
	}
}

}


