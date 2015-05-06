#ifndef FONT_H
#define FONT_H

namespace venk {

class FontService;

class Font {

	friend class FontService;

public:
	Font(const std::string& fontName);

	//////////////////////////////////////////////////////////////////////////////
	//
	// CHARACTER TO GLYPH-INDEX CONVERSIOn
	// If you're going to perform multiple operations on the same character
	// and you want a speed-up, call this function with the character you're
	// going to process, then use glyph-based functions instead of the
	// codepoint-based functions.
	int find_glyph_index(int unicode_codepoint);

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
	float scale_for_pixel_height(float pixels);

	// computes a scale factor to produce a font whose EM size is mapped to
	// 'pixels' tall. This is probably what traditional APIs compute, but
	// I'm not positive.
	float scale_for_mapping_em_to_pixels(float pixels);

	// ascent is the coordinate above the baseline the font extends; descent
	// is the coordinate below the baseline the font extends (i.e. it is typically negative)
	// lineGap is the spacing between one row's descent and the next row's ascent...
	// so you should advance the vertical position by "*ascent - *descent + *lineGap"
	//   these are expressed in unscaled coordinates, so you must multiply by
	//   the scale factor for a given size
	void get_font_v_metrics(int& ascent, int& descent, int& lineGap);

	// the bounding box around all possible characters
	void get_font_bounding_box(int& x0, int& y0, int& x1, int& y1);

	// leftSideBearing is the offset from the current horizontal position to the left edge of the character
	// advanceWidth is the offset from the current horizontal position to the next horizontal position
	//   these are expressed in unscaled coordinates
	void get_codepoint_h_metrics(int codepoint, int& advanceWidth, int& leftSideBearing);

	// an additional amount to add to the 'advance' value between ch1 and ch2
	int get_codepoint_kern_advance(int ch1, int ch2);

	// Gets the bounding box of the visible part of the glyph, in unscaled coordinates
	int get_codepoint_box(int codepoint, int& x0, int& y0, int& x1, int &y1);

	// as above, but takes one or more glyph indices for greater efficiency
	void get_glphp_HMetrics(int glyph_index, int& advanceWidth, int& leftSideBearing);

	int  get_glyph_kern_advance(int glyph1, int glyph2);

	int get_glyph_box(int glyph_index, int& x0, int& y0, int& x1, int& y1);

	~Font();

private:
	stbtt_fontinfo *font;
	void           *buf;
};

class FontService : public Service< FontService > {

public:
	typedef FontService* ServicePtr_t;

	static bool initialise(FontService* self);

	static bool shutdown(FontService* self);
	
private:
	typedef std::unordered_map< std::string, std::weak_ptr<Font> > FontLookupTable_t;
	FontLookupTable_t fontTable;

};


}
#endif
