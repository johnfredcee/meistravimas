#ifndef IMAGES_H_INCLUDED
#define IMAGES_H_INCLUDED

namespace venk
{

	struct Image {
		Uint8 *pixels;
		int    width;
		int    height;
		int    channels;
	};

	std::shared_ptr<Image> image_load(const std::string& fileName);
	std::shared_ptr<SDL_Surface> surface_from_image(std::shared_ptr<Image> img);

}

#endif
