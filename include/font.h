#ifndef FONT_H
#define FONT_H

namespace venk {

class FontService : public Service<FontService> {

private:
	::NVGcontext* vg;	

public:
	struct Bounds
	{
		float minx;
		float miny;
		float maxx;
		float maxy;
	};

	struct Metrics
	{
		float ascender;
		float descender;
		float lineh;
	};

	typedef FontService* ServicePtr_t;

	static bool initialise(FontService* self);

	static bool shutdown(FontService* self);

	int createFont(const char* name, const char* filename);	

	int findFontByName(const char* name);

	std::shared_ptr<Metrics> getFontMetrics();

	void setCurrentFont(int font);

	std::shared_ptr<Bounds> getTextBounds(float x, float y, const char* string, const char* end);

	 void drawText(float x, float y, const char* string, const char* end);

};

}
#endif
