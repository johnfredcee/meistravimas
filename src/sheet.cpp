#include <iostream>
#include <string>
#include <memory>
#include <SDL.h>
#include <SDL_rwops.h>
#include <SOIL.h>
#include <images.h>
#include <res_path.h>
#include "images.h"
#include "sheet.h"

namespace venk
{

std::shared_ptr<sheet> sheet_load(const std::string& fileName)
{
	std::shared_ptr<Image> img(image_load(fileName));
	if (img) {
		std::shared_ptr<sheet> result(std::make_shared<sheet>());
		result->img = img;
		return result;
	} else {
		return std::shared_ptr<sheet>();
	}
}

bool sheet_cut(std::shared_ptr<sheet> sheet, std::shared_ptr<SDL_Renderer> target,
               const sheet_cut_params& params)
{
	std::shared_ptr<SDL_Surface> s(surface_from_image(sheet->img));
	if (s) {
		auto texture_deleter = [](SDL_Texture *t) {
			if (t) SDL_DestroyTexture(t);
		};
		std::shared_ptr<SDL_Texture> tex(SDL_CreateTextureFromSurface(target.get(), s.get()),
		                                 texture_deleter);
		sheet->tex = tex;
		sheet->target = target;
		sheet->params = params;
		return true;
	}
	return false;
}

void render_cell(std::shared_ptr<sheet> sheet, Uint16 x, Uint16 y)
{
	if ((sheet) && (sheet->target) && (sheet->tex)) {
		SDL_Rect src_rect;
		SDL_Rect dest_rect;
		Uint16 sx = sheet->params.xoffset + ( x * sheet->params.xspacing);
		Uint16 sy = sheet->params.yoffset + ( y * sheet->params.yspacing);
		src_rect.x = sx;
		src_rect.y = sy;
		src_rect.w = sheet->params.tileWidth;
		src_rect.h = sheet->params.tileHeight;
		dest_rect.x = x;
		dest_rect.y = y;
		dest_rect.w = src_rect.w;
		dest_rect.h = src_rect.h;

		SDL_RenderCopy(sheet->target.get(), sheet->tex.get(), &src_rect, &dest_rect);
	}
};

}
