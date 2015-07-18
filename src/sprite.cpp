
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


Uint32								          SpriteService::nextBatchId = 1;
std::shared_ptr<Program> SpriteService::spriteShader = std::shared_ptr<Program>();
SpriteService::SpriteBatchArray_t             SpriteService::batches;
SpriteService::SpriteBatchArray_t::iterator   SpriteService::batchWalker;
Matrix44									  SpriteService::projection;


bool SpriteService::initialise(SpriteService* self) {
	(void) self;
	ServiceCheckout<ProgramService> programs;
	spriteShader = programs->loadProgram("sprite");
	projection.ortho(0.0f, (float) ::screen_width, (float) ::screen_height, 0.0f, -1.0f, 1.0f);
	SDL_assert(spriteShader->isValid());
	batchWalker = batches.begin();
	return true;
}


std::shared_ptr<SpriteBatch> SpriteService::addBatch(Uint32 size, std::shared_ptr<Texture> texture) {
	nextBatchId++;
	std::shared_ptr<SpriteBatch> sprite_batch(std::make_shared<SpriteBatch>(nextBatchId, size, texture));
	batches.push_back(sprite_batch);
	return std::shared_ptr<SpriteBatch>(sprite_batch);
}

void SpriteService::beginBatchWalk() {
	batchWalker = batches.begin();
}

std::shared_ptr<SpriteBatch> SpriteService::nextBatch() {
	if(batchWalker != batches.end()) {
		std::shared_ptr<SpriteBatch> result(*batchWalker);
		++batchWalker;
		return result;
	}
	return std::shared_ptr<SpriteBatch>();
}


bool SpriteService::shutdown(SpriteService* self) {
	(void) self;
	return true;
}


// SpriteBatch methods
SpriteBatch::SpriteBatch(Uint32 batchId, Uint32 count, std::shared_ptr<Texture> batchTexture) : texture(batchTexture), id(batchId), nSprites(count), nUsed(0) {
	vertexBufferB = std::make_shared<BufferBuilder>(GL_FLOAT, 3);
	uvBufferB = std::make_shared<BufferBuilder>(GL_FLOAT, 2);
	indexBufferB = std::make_shared<BufferBuilder>(GL_UNSIGNED_SHORT, 1);
	std::vector<GLushort> indices = { 0, 1, 2, 2, 3, 0 };
	GLushort offset = 0;
	for(Uint32 i = 0; i < nSprites; ++i) {
		vertexBufferB->addVec3f(-0.5f, -0.5f, 0.0f);
		vertexBufferB->addVec3f(-0.5f,  0.5f, 0.0f);
		vertexBufferB->addVec3f(0.5f,   0.5f, 0.0f);
		vertexBufferB->addVec3f(0.5f,  -0.5f, 0.0f);		
		uvBufferB->addVec2f(0.0f, 1.0f);
		uvBufferB->addVec2f(0.0f, 0.0f);
		uvBufferB->addVec2f(1.0f, 0.0f);
		uvBufferB->addVec2f(1.0f, 1.0f);
		for(auto i = indices.begin(); i != indices.end(); i++) {
			indexBufferB->addVec1ui(*i + offset);
		}
		offset += 4;
	}
	vertexBuffer = vertexBufferB->make_buffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW);
	uvBuffer = uvBufferB->make_buffer(GL_ARRAY_BUFFER, GL_STREAM_DRAW);
	indexBuffer = indexBufferB->make_buffer(GL_ELEMENT_ARRAY_BUFFER, GL_STREAM_DRAW);	
	return;
}

SpriteBatch::~SpriteBatch() {
}

std::weak_ptr<Sprite>  SpriteBatch::addSprite(float x, float y, float width, float height, int sheetx, int sheety) {
	SDL_assert(sprites.size() < nSprites);
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
	sprite->u0 = (float) sheetx / (float) texture->width;
	sprite->u1 = (float)(sheetx +  width) / (float) texture->width;
	sprite->v0 = 1.0f - ((float) sheety / (float) texture->height);
	sprite->v1 = 1.0f - (((float) sheety + (float) height) / (float) texture->height);
	sprite->x = x;
	sprite->y = y;
	sprite->w = width;
	sprite->h = height;
	sprite->scale = 1.0f;
	sprites.push_back(sprite);
	nUsed++;
	return std::weak_ptr<Sprite>(sprite);
}

void SpriteBatch::render(double alpha, SDL_Window* window, SDL_Renderer* renderer) {
	(void) alpha;
	(void) window;
	(void) renderer;
	SpriteService::spriteShader->use();
	float scl = 1.0f;
	for(auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
		auto index = sprite - sprites.begin();
		auto sprptr = sprite->get();
		Uint32 vIndex = index * 4;
		vertexBufferB->setVec3f(vIndex, sprptr->x, sprptr->y, -0.1f);
		vertexBufferB->setVec3f(vIndex+1, sprptr->x, sprptr->y + sprptr->h, -0.1f);
		vertexBufferB->setVec3f(vIndex+2, sprptr->x + sprptr->w , sprptr->y + sprptr->h, -0.1f);
		vertexBufferB->setVec3f(vIndex+3, sprptr->x + sprptr->w, sprptr->y, 0.0f);
		uvBufferB->setVec2f(vIndex, sprptr->u0, sprptr->v1);
		uvBufferB->setVec2f(vIndex+1, sprptr->u0, sprptr->v0);
		uvBufferB->setVec2f(vIndex+2, sprptr->u1, sprptr->v0);
		uvBufferB->setVec2f(vIndex+3, sprptr->u1, sprptr->v1);		
	}	
	vertexBufferB->update_buffer(vertexBuffer);
	uvBufferB->update_buffer(uvBuffer);
	for(auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite) {
		std::shared_ptr<Sprite> sprptr(*sprite);
		scl = sprptr->scale;
		SpriteService::spriteShader->setUniform1f("vScale", &scl);
		SpriteService::spriteShader->setUniformMat4f("mModelToClip", SpriteService::projection.elements);
		SpriteService::spriteShader->setUniformSampler("textureMap", 0, texture.get()); 
		vertexBuffer->bindAttribute(SpriteService::spriteShader.get(), "vVertex");
		 uvBuffer->bindAttribute(SpriteService::spriteShader.get(), "vUV"); 
		indexBuffer->bindIndices();
		indexBuffer->drawRange(GL_TRIANGLES, 6, 6 * sizeof(GLushort) * (sprite - sprites.begin()));
	}
	SpriteService::spriteShader->unUse();
}

Sprite::Sprite() {
}

Sprite::~Sprite() {
}


void SpriteBatch::update(double t, double dt) {
	(void) t;
	(void) dt;
}

float Sprite::getScale() const {
	return scale;
}

void Sprite::setScale(float newScale) {
	scale = newScale;
}

float Sprite::getX() const {
	return x;
}

float Sprite::getY() const {
	return y;
}

void Sprite::setXY(float newX, float newY) {
	x = newX;
	y = newY;
}

}

