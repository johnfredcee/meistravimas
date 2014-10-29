
#include <iostream>
#include <memory>
#include <SDL.h>
#include <images.h>
#include <SOIL.h>
#include <res_path.h>

namespace venk
{

    std::shared_ptr<Image> image_load(const std::string& fileName)
    {
		auto image_deleter = [](Image *i) {
			if (i) {
				SOIL_free_image_data(i->pixels);
				delete i;
			}
		};
		std::shared_ptr<Image> img(new Image, image_deleter);
		SDL_RWops *rwops = SDL_RWFromFile(fileName.c_str(), "rb");
		if (rwops != nullptr) {
			Sint64 size = SDL_RWsize(rwops);
			if (size != -1L) {
				void *buf = SDL_malloc(size);
				size_t read = SDL_RWread(rwops, buf, 1, size);
				if (read == size) {
					img->pixels = SOIL_load_image_from_memory((const unsigned char*)buf, size, &img->width, &img->height, &img->channels, SOIL_LOAD_AUTO);
					if (img->pixels != nullptr) {
						SDL_RWclose(rwops);
						return img;
					} else {
						std::cerr << "Creating " << fileName << " failed" << std::endl;
					}
				} else {
					std::cerr << "Reading " << fileName << " failed" << std::endl;
				}
				SDL_free(buf);
			} else {
				std::cerr << "Probing " << fileName << " failed" << std::endl;
			}
		} else {
			std::cerr << "Opening " << fileName << " failed" << std::endl;
		}
		SDL_RWclose(rwops);
		return nullptr;
    }

    std::shared_ptr<SDL_Surface> surface_from_image(std::shared_ptr<Image> img)
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

		auto surface_deleter = [](SDL_Surface* s) {
			if(s) SDL_FreeSurface(s);
		};
		std::shared_ptr<SDL_Surface> result(SDL_CreateRGBSurfaceFrom(img->pixels, img->width, img->height, img->channels * 8, img->width * img->channels, rmask, gmask, bmask, amask ),
											surface_deleter);
		SDL_SetSurfaceBlendMode(result.get(), SDL_BLENDMODE_NONE);
		return result;
    }

}
