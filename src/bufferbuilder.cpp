
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>
#include <SDL_opengl.h>
#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "serviceregistry.h"
#include "buffer.h"
#include "bufferbuilder.h"

namespace venk
{

BufferBuilder::BufferBuilder(GLenum type, GLsizei componentCount)
{
	mBufferSize		= 0;
	mBuffer			= (Uint8*)SDL_malloc(1024);
	mBufferCapacity = 1024;
	mItemCount		= 0;
	mType			= type;
	mComponentCount = componentCount;
}

BufferBuilder::~BufferBuilder()
{
	SDL_free(mBuffer);
}

void BufferBuilder::growBuffer()
{
	Uint8 *newBuffer  = (Uint8*) SDL_malloc(mBufferCapacity * 2);
	SDL_memcpy(newBuffer, mBuffer, mBufferCapacity);
	SDL_free(mBuffer);
	mBuffer = newBuffer;
	mBufferCapacity = mBufferCapacity * 2;
	return;
}

void BufferBuilder::addVec1f(GLfloat x)
{
	SDL_assert(mType == GL_FLOAT);
	SDL_assert(mComponentCount == 1);
	if ((GLsizei)(mBufferSize + sizeof(GLfloat)) >= mBufferCapacity)
		growBuffer();
	(*(GLfloat*)(mBuffer + mBufferSize)) = x;
	mBufferSize += sizeof(GLfloat);
	mItemCount++;
}

void BufferBuilder::addVec2f(GLfloat x, GLfloat y)
{
	SDL_assert(mType == GL_FLOAT);
	SDL_assert(mComponentCount == 2);
	if ((GLsizei)(mBufferSize + sizeof(GLfloat) * 2)  >= mBufferCapacity)
		growBuffer();
	(*(GLfloat*)(mBuffer + mBufferSize)) = x;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = y;
	mBufferSize += sizeof(GLfloat);
	mItemCount++;
}

void BufferBuilder::addVec2f(const Vector2d& v)
{
	addVec2f(v.x(), v.y());
}

void BufferBuilder::addVec3f(GLfloat x, GLfloat y, GLfloat z)
{
	SDL_assert(mType == GL_FLOAT);
	SDL_assert(mComponentCount == 3);
	if ((GLsizei)(mBufferSize + sizeof(GLfloat) * 3) >= mBufferCapacity)
		growBuffer();
	(*(GLfloat*)(mBuffer + mBufferSize)) = x;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = y;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = z;
	mBufferSize += sizeof(GLfloat);
	mItemCount++;
}

void BufferBuilder::addVec3f(const Vector3d& v)
{
	addVec3f(v.x(), v.y(), v.z());
}

void BufferBuilder::addVec4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	SDL_assert(mType == GL_FLOAT);
	SDL_assert(mComponentCount == 4);
	if ((GLsizei)(mBufferSize + sizeof(GLfloat) * 4) >= mBufferCapacity)
		growBuffer();
	(*(GLfloat*)(mBuffer + mBufferSize)) = x;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = y;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = z;
	mBufferSize += sizeof(GLfloat);
	(*(GLfloat*)(mBuffer + mBufferSize)) = w;
	mBufferSize += sizeof(GLfloat);
	mItemCount++;
}

void BufferBuilder::addVec1ui(GLushort i)
{
	SDL_assert(mType == GL_UNSIGNED_SHORT);
	SDL_assert(mComponentCount == 1);
	if ((GLsizei)(mBufferSize + sizeof(GLushort)) >= mBufferCapacity)
		growBuffer();
	(*(GLushort*)(mBuffer + mBufferSize)) = i;
	mBufferSize += sizeof(GLushort);
	mItemCount++;
}

void BufferBuilder::addVec2ui(GLushort i, GLushort j)
{
	SDL_assert(mType == GL_UNSIGNED_SHORT);
	SDL_assert(mComponentCount == 2);
	if ((GLsizei)(mBufferSize + sizeof(GLushort) * 2) >= mBufferCapacity)
		growBuffer();
	(*(GLushort*)(mBuffer + mBufferSize)) = i;
	mBufferSize += sizeof(GLushort);
	(*(GLushort*)(mBuffer + mBufferSize)) = j;
	mBufferSize += sizeof(GLushort);
	mItemCount++;
}

void BufferBuilder::addVec3ui(GLushort i, GLushort j, GLushort k)
{
	SDL_assert(mType == GL_UNSIGNED_SHORT);
	SDL_assert(mComponentCount == 3);
	if ((GLsizei)(mBufferSize + sizeof(GLushort) * 3) >= mBufferCapacity)
		growBuffer();
	(*(GLushort*)(mBuffer + mBufferSize)) = i;
	mBufferSize += sizeof(GLushort);
    (*(GLushort*)(mBuffer + mBufferSize)) = j;
	mBufferSize += sizeof(GLushort);
    (*(GLushort*)(mBuffer + mBufferSize)) = k;
	mBufferSize += sizeof(GLushort);
	mItemCount++;
}

const void *BufferBuilder::getData()
{
	return (const void*)mBuffer;
}

GLsizei BufferBuilder::itemCount()
{
	return mItemCount;
}

std::shared_ptr<Buffer> BufferBuilder::make_buffer(GLenum target, GLenum usage)
{
	ServiceCheckout<BufferManagerService> buffers;
    return buffers->make_buffer(target, (void*) mBuffer, mType, mItemCount, mComponentCount, usage);
}

}
