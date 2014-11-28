
#include <vector>
#include <unordered_map>
#include <memory>
#include <list>
#include <array>

#include <SDL.h>
#include <SDL_net.h>
#include <SDL_opengl.h>
#include <scheme-private.h>


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
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "panel.h"
#include "gui.h"

namespace venk
{

std::shared_ptr<Panel> GuiService::root = std::shared_ptr<Panel>();

bool GuiService::initialise(GuiService* self)
{
	ServiceCheckout<ProgramService> programs;
	self->gui = programs->loadProgram("gui");
	self->guiTransform.identity();
	return true;
}

bool GuiService::shutdown(GuiService* self)
{
	self->gui = nullptr;
	return true;
}

bool GuiService::render()
{
	gui->use();
	if (root)
		root->render();
	gui->unUse();
	return true;
}

}
