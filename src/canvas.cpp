
#include <vector>
#include <unordered_map>
#include <memory>
#include <list>
#include <array>
#include <functional>

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_net.h>
#include <SDL_opengl.h>
#include <scheme-private.h>
#include <physfs.h>
#include <res_path.h>
#include <scheme-defs.h>
#include <scheme-private.h>
#include <scheme.h>

#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "quat.h"
#include "colour.h"
#include "matrix44.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "image.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "scripting.h"
#include "tinyscm_if.h"
#include "canvas.h"

extern "C"
{
	NVGcontext* get_canvas_context()
	{
		return nullptr;
	}
};

namespace venk
{

void CanvasService::begin_frame(int width, int height)
{
	glEnable(GL_STENCIL_TEST);
	::nvgBeginFrame(vg, width, height, 1.0f);
}

void CanvasService::end_frame()
{
 	::nvgEndFrame(vg);	
    glDisable(GL_STENCIL_TEST);
}

bool CanvasService::render(int width, int height)
{
	begin_frame(width, height);

    nvgResetTransform(vg);
    nvgTranslate( vg, 300.0f, 300.0f );
    nvgBeginPath( vg );
    nvgRect( vg, -100, -25, 200, 50 );
    nvgStrokeColor( vg, nvgRGBA( 255, 0, 0, 100 ) );
    nvgStrokeWidth( vg, 1.0f );
    nvgStroke( vg );
	end_frame();
	return true;
}

bool CanvasService::initialise(CanvasService* self)
{
	self->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
 	return true;
}

bool CanvasService::shutdown(CanvasService* self)
{
	nvgDeleteGL3(self->vg);
	return true;
}

}


