#ifndef SHEET_H_INCLUDED
#define SHEETs_H_INCLUDED

namespace venk
{
struct sheet_cut_params {
	Uint16 xtiles;
	Uint16 ytiles;
	Uint16 xoffset;
	Uint16 yoffset;
	Uint16 tileWidth;
	Uint16 tileHeight;
	Uint16 xspacing;
	Uint16 yspacing;
};

struct sheet {
	std::shared_ptr<Image>        img;
	std::shared_ptr<SDL_Texture>  tex;
	std::shared_ptr<SDL_Renderer> target;
	sheet_cut_params              params;
};


std::shared_ptr<sheet> sheet_load(const std::string& fileName);
bool sheet_cut(std::shared_ptr<SDL_Renderer> target, const sheet_cut_params& params);
void render_cell(std::shared_ptr<sheet>, Uint16 x, Uint16 y);
}
#endif
