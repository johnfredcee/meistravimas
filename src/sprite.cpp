
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_net.h>
#include <scheme-private.h>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "quat.h"
#include "matrix44.h"
#include "image.h"
#include "texture.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "bufferbuilder.h"
#include "sprite.h"

extern int screen_width;
extern int screen_height;

namespace venk {

std::shared_ptr<Buffer> SpriteService::vertexBuffer = std::shared_ptr<Buffer>();
std::shared_ptr<Buffer> SpriteService::uvBuffer = std::shared_ptr<Buffer>();
std::shared_ptr<Buffer> SpriteService::indexBuffer = std::shared_ptr<Buffer>();	
std::shared_ptr<Program> SpriteService::spriteShader = std::shared_ptr<Program>();
Matrix44 SpriteService::projection;

SpriteService::SpriteTable_t SpriteService::sprites;

bool SpriteService::initialise(SpriteService* self)
{
	(void) self;
	ServiceCheckout<ProgramService> programs;
	spriteShader = programs->loadProgram("sprite");
	projection.ortho(0.0f, (float) ::screen_width, 0.0f, (float) ::screen_height, -1.0f, 1.0f);
	SDL_assert(spriteShader->isValid());	
	return true;
}

std::shared_ptr<Sprite> SpriteService::createSprite(std::shared_ptr<Texture> texture, int sheetx, int sheety, int w, int h) {
	std::shared_ptr<Sprite> result = std::make_shared<Sprite>(texture, sheetx, sheety, w, h);
	std::weak_ptr<Sprite> ref(result);

	SpriteService::sprites.push_back(ref);
	return result;
}

void SpriteService::render(double alpha, SDL_Window* window, SDL_Renderer* renderer)
{
	(void) alpha;
	(void) window;
	(void) renderer;
	
	std::shared_ptr<BufferBuilder> vertexBufferB(std::make_shared<BufferBuilder>(GL_FLOAT, 3));
	std::shared_ptr<BufferBuilder> uvBufferB(std::make_shared<BufferBuilder>(GL_FLOAT, 2));
	std::shared_ptr<BufferBuilder> indexBufferB(std::make_shared<BufferBuilder>(GL_UNSIGNED_SHORT, 1));

	std::vector<Vector2d> vertices = { { -0.5f, -0.5f },
									   { -0.5f, 0.5f },
									   { 0.5f, 0.5f },
									   { 0.5f, -0.5f } };

	std::vector<GLushort> indices = { 0, 1, 2, 2, 3, 0 };

	GLushort offset = 0;
	for(auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
		if (!sprite->expired()) {
			std::shared_ptr<Sprite> sprptr(sprite->lock());
			for(auto i = vertices.begin(); i != vertices.end(); i++) {
				float x = i->x() + sprptr->x;
				float y = i->y() + sprptr->y;				
				vertexBufferB->addVec3f(x,y, 0.0f);
			}
			uvBufferB->addVec2f(sprptr->u0, sprptr->v1);
			uvBufferB->addVec2f(sprptr->u0, sprptr->v0);
			uvBufferB->addVec2f(sprptr->u1, sprptr->v0);
			uvBufferB->addVec2f(sprptr->u1, sprptr->v1);
			for(auto i = indices.begin(); i != indices.end(); i++) {
				indexBufferB->addVec1ui(*i + offset);
			}
			offset += 4;
		}		
	}		
	vertexBuffer = vertexBufferB->make_buffer(GL_ARRAY_BUFFER);
	uvBuffer = uvBufferB->make_buffer(GL_ARRAY_BUFFER);
	indexBuffer = indexBufferB->make_buffer(GL_ELEMENT_ARRAY_BUFFER);	
	SpriteService::spriteShader->use();
	float scl = 1.0f;
	for(auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
		if (!sprite->expired()) {
			std::shared_ptr<Sprite> sprptr(sprite->lock());
			scl = sprptr->scale;
			SpriteService::spriteShader->setUniform1f("vScale", &scl);
			SpriteService::spriteShader->setUniformMat4f("mModelToClip", SpriteService::projection.elements);
			SpriteService::spriteShader->setUniformSampler("textureMap", 0, sprptr->texture.get());	
			SpriteService::vertexBuffer->bindAttribute(SpriteService::spriteShader.get(), "vVertex");
			SpriteService::uvBuffer->bindAttribute(SpriteService::spriteShader.get(), "vUV");
			SpriteService::indexBuffer->bindIndices();
			SpriteService::indexBuffer->draw(GL_TRIANGLES);
		}
	}	
	SpriteService::spriteShader->unUse();	
}

void SpriteService::update(double t, double dt)
{
	(void) t;
	(void) dt;
	
	for(auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
		if (sprite->expired()) {
			sprite = sprites.erase(sprite);
		}
	}
}

bool SpriteService::shutdown(SpriteService* self)
{
	(void) self;
	return true;
}


Sprite::Sprite(std::shared_ptr<Texture> tex, int sheetx, int sheety, float w, float h) :  x(0.0f), y(0.0f), scale(1.0f), texture(tex)
{
	u0 = (float) sheetx / (float) tex->width;
	u1 = (float) (sheetx +  w) / (float) tex->width;
	v0 = 1.0f - ((float) sheety / (float) tex->height);
	v1 = 1.0f - (((float) sheety + (float) h) / (float) tex->height);
}

Sprite::~Sprite()
{
}

void Sprite::render(double alpha, SDL_Window* window, SDL_Renderer* renderer)
{
	(void) alpha;
	(void) window;
	(void) renderer;
}

void Sprite::update(double t, double dt)
{
	(void) t;
	(void) dt;
}

float Sprite::getScale() const
{
	return scale;
}

void Sprite::setScale(float newScale)
{
	scale = newScale;
}

float Sprite::getX() const
{
	return x;
}

float Sprite::getY() const
{
	return y;
}

void Sprite::setXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

}
