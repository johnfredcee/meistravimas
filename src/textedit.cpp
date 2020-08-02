
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
#include "nanovg.h"
#include "canvas.h"
#include "font.h"
#include "textedit_if.h"
#include "textedit.h"

namespace venk {

	bool TextEditService::initialise(TextEditService* self)
	{
		(void) self;
		ServiceCheckout<CanvasService> canvas;
		self->vg = canvas->vg;
		self->state = textedit_initialise();
		return true;
	}

	bool TextEditService::shutdown(TextEditService* self) {
		(void) self;
		self->vg = nullptr;
		SDL_free(self->state);
		self->state = nullptr;

		return true;
	}

}