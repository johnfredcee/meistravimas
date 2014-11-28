
#include <vector>
#include <unordered_map>
#include <memory>
#include <list>

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

Panel::Panel() : parent(nullptr), x(-1.0f), y(-1.0f), w(2.0f), h(2.0f)
{
	panelTransform.identity();
}
		
Panel::Panel(std::shared_ptr<Panel> p, float leftx, float topy, float width, float height) : parent(p), x(leftx), y(topy), w(width), h(height)
{
	if (parent)
		parent->add_child(this);
	ServiceCheckout<BufferManagerService> buffers;
	panelVertices[0] = leftx;
	panelVertices[1] = topy;
	panelVertices[2] = leftx;
	panelVertices[3] = topy + height;
	panelVertices[4] = leftx + width;
	panelVertices[5] = topy + height;
	panelVertices[6] = leftx + width;
	panelVertices[7] = topy;
	vb = buffers->make_buffer(GL_ARRAY_BUFFER, (const void*) &panelVertices[0], GL_FLOAT, 4, 2, GL_STATIC_DRAW);
	calcluate_transform();
} 

void Panel::add_child(Panel* c) {
	children.push_front(c);
}

void Panel::remove_child(Panel *c) {
	children.remove(c);
}

void Panel::set_parent(std::shared_ptr<Panel> p)
{
	SDL_assert(p);
	p->add_child(this);
	if (parent)
		parent->remove_child(this);
	parent = p;
	calcluate_transform();
}

std::shared_ptr<Panel> Panel::get_parent()
{
	return parent;
}

void Panel::set_color(float r, float g, float b)
{
	panelColor.elements[0] = r;
	panelColor.elements[1] = g;
	panelColor.elements[2] = b;
	panelColor.elements[3] = 0.0f;
	return;
}

void Panel::calcluate_transform()
{
	std::shared_ptr<Panel> parent = get_parent();
	Matrix44 working_transform;
	Matrix44 final_transform;
	working_transform.identity();
	while (parent) {
		Vector3d translation(parent->x + (parent->w * 0.5f), parent->y * (parent->h * 0.5f), 0.0f);
		Vector3d scale(parent->w * 0.5f, parent->h * 0.5f, 1.0f);
		working_transform.translation(translation);
		working_transform.scale(scale);
		final_transform = working_transform * final_transform;
		parent = parent->get_parent();
	};
	panelTransform = working_transform;
}

bool Panel::render()
{
	ServiceCheckout<GuiService> gui;
	gui->program()->setUniform4f("vColor", &panelColor.elements[0]);
	gui->program()->setUniformMat4f("mPanel", &panelTransform.elements[0]);
	vb->bindAttribute(gui->program().get(), "vVertex");
	vb->drawImmediate(GL_TRIANGLE_STRIP);
	return true;
}

};
