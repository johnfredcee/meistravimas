
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <functional>
#include <cctype>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_net.h>
#include <physfs.h>
#include <physfsrwops.h>
#include <res_path.h>
#include <scheme-defs.h>
#include <scheme-private.h>
#include <scheme.h>
#include <stb_truetype.h>
#include "hashstring.h"
#include "locator.h"   
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "scripting.h"
#include "tinyscm_if.h"
#include "image.h"
#include "font.h"

namespace venk {


bool FontService::initialise(FontService* self)
{
	(void) self;
	// ServiceCheckout<ScriptingService> scripting;
	// scheme* sc = scripting->get_scheme();
	// register_font_functions(sc);
	return true;
}

bool FontService::shutdown(FontService* self) {
	(void) self;
	// self->fontTable.clear();
	return true;
}


// -------------------- Font methods --------------------

Font::Font(const std::string& fontName) {
	std::string fullFileName =  "fonts/" + fontName;
	SDL_RWops *rwops =  PHYSFSRWOPS_openRead(fullFileName.c_str());
	if (rwops != nullptr) {
		Sint64 size = SDL_RWsize(rwops);
		if (size != -1L) {
			void *buf = SDL_malloc(size);
			size_t read = SDL_RWread(rwops, buf, 1, size);
			if (read == size) {
				int result = stbtt_InitFont(font, (const unsigned char*) buf, 0);
				if (result == 0) {
					SDL_RWclose(rwops);
					return;
				} else {
					std::cerr << "Creating " << fontName << " failed" << std::endl;
				}
			} else {
				std::cerr << "Reading " << fontName << " failed" << std::endl;
			}
			SDL_free(buf);
		} else {
			std::cerr << "Probing " << fontName << " failed" << std::endl;
		}
		SDL_RWclose(rwops);
	} else {
		std::cerr << "Opening " << fontName << " failed" << std::endl;
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER TO GLYPH-INDEX CONVERSIOn
// If you're going to perform multiple operations on the same character
// and you want a speed-up, call this function with the character you're
// going to process, then use glyph-based functions instead of the
// codepoint-based functions.
int Font::find_glyph_index(int unicode_codepoint) {
	return stbtt_FindGlyphIndex(font, unicode_codepoint);
}

//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER PROPERTIES
//
// computes a scale factor to produce a font whose "height" is 'pixels' tall.
// Height is measured as the distance from the highest ascender to the lowest
// descender; in other words, it's equivalent to calling stbtt_GetFontVMetrics
// and computing:
//       scale = pixels / (ascent - descent)
// so if you prefer to measure height by the ascent only, use a similar calculation.
float Font::scale_for_pixel_height(float pixels) {
	return stbtt_ScaleForPixelHeight(font, pixels);
}

// computes a scale factor to produce a font whose EM size is mapped to
// 'pixels' tall. This is probably what traditional APIs compute, but
// I'm not positive.
float Font::scale_for_mapping_em_to_pixels(float pixels) {
	return stbtt_ScaleForMappingEmToPixels(font,  pixels);
}

// ascent is the coordinate above the baseline the font extends; descent
// is the coordinate below the baseline the font extends (i.e. it is typically negative)
// lineGap is the spacing between one row's descent and the next row's ascent...
// so you should advance the vertical position by "*ascent - *descent + *lineGap"
//   these are expressed in unscaled coordinates, so you must multiply by
//   the scale factor for a given size
void Font::get_font_v_metrics(int& ascent, int& descent, int& lineGap) {
	stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
}

// the bounding box around all possible characters
void Font::get_font_bounding_box(int& x0, int& y0, int& x1, int& y1) {
	stbtt_GetFontBoundingBox(font, &x0, &y0, &x1, &y1);
}

// leftSideBearing is the offset from the current horizontal position to the left edge of the character
// advanceWidth is the offset from the current horizontal position to the next horizontal position
//   these are expressed in unscaled coordinates
void Font::get_codepoint_h_metrics(int codepoint, int& advanceWidth, int& leftSideBearing) {
	stbtt_GetCodepointHMetrics(font, codepoint, &advanceWidth, &leftSideBearing);
}

// an additional amount to add to the 'advance' value between ch1 and ch2
int Font::get_codepoint_kern_advance(int ch1, int ch2) {
	return stbtt_GetCodepointKernAdvance(font, ch1, ch2);
}

// Gets the bounding box of the visible part of the glyph, in unscaled coordinates
int Font::get_codepoint_box(int codepoint, int& x0, int& y0, int& x1, int &y1) {
	return stbtt_GetCodepointBox(font,codepoint, &x0,  &y0,  &x1,  &y1);
}

// as above, but takes one or more glyph indices for greater efficiency
void Font::get_glphp_HMetrics(int glyph_index, int& advanceWidth, int& leftSideBearing) {
	stbtt_GetGlyphHMetrics(font, glyph_index, &advanceWidth, &leftSideBearing);
}

int  Font::get_glyph_kern_advance(int glyph1, int glyph2) {
	return  stbtt_GetGlyphKernAdvance(font, glyph1, glyph2);
}

int Font::get_glyph_box(int glyph_index, int& x0, int& y0, int& x1, int& y1) {
	return stbtt_GetGlyphBox(font, glyph_index, &x0, &y0, &x1, &y1);
}


Font::~Font() {
	SDL_free(buf);
}

}
