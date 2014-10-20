

#include <unordered_map>
#include <memory>
#include <glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "utils.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "program.h"
#include "buffer.h"

namespace venk
{

bool BufferManagerService::initialise(BufferManagerService* self)
{
	(void) self;
	return true;
}

bool BufferManagerService::shutdown(BufferManagerService* self)
{
	(void) self;
	return true;
}

std::shared_ptr<Buffer> BufferManagerService::make_buffer(GLenum		target,
										const void  *bufferData,
										GLenum		type,
										GLsizei		count,
										GLsizei		componentCount,
										GLenum		usage)
{
	return std::make_shared<Buffer>(target, bufferData, type, count, componentCount, usage);
}


/**
 * Construct a vertex or index buffer
 * @param target GL_ARRAY_BUFFER for a buffer of elements, GL_ELEMENT_ARRAY_BUFFER for a buffer of indices
 * @param bufferData Pointer to the actual buffer data
 * @param type Data type of the data eg. GL_FLOAT
 * @param count Number of items in the buffer
 * @param componentCount Number of components in the item (eg 3 for verts, 2 for uv)
 * @param usage Usage hint
 */
Buffer::Buffer(GLenum		target,
			   const void  *bufferData,
			   GLenum	type,
			   GLsizei		count,
			   GLsizei		componentCount,
			   GLenum		usage)	: mTarget(target), mSize(count), mComponentCount(componentCount), mType(type) 
{

	GLsizei typeSize = getTypeSize(type);
	glGenBuffers(1, &mBuffer);
	SDL_assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(mTarget, mBuffer);
	SDL_assert(glGetError() == GL_NO_ERROR);
	glBufferData(mTarget, count * typeSize * componentCount, bufferData, usage);
	SDL_assert(glGetError() == GL_NO_ERROR);
	return;
}

/**
 * Update data in the buffer (all of it)
 * @param bufferData Data to replace data in buffer with
 */
void Buffer::update(const void *bufferData)
{
	// some drivers crash if we don't do this.
	glBindBuffer(mTarget, mBuffer);
	SDL_assert(glGetError() == GL_NO_ERROR);
	glBufferSubData(mTarget, 0, mSize * getTypeSize(mType) * mComponentCount, bufferData);
	SDL_assert(glGetError() == GL_NO_ERROR);
	return;
}

/**
 * Bind the buffer to a shader attribute
 * @param program shader program with attribute we wish to bind
 * @param attributeIndex index of attribute associated with program
 * @param componentCount number of components in data (eg 3 for vert, 2 for uv)
 */
void Buffer::bindAttribute(Program* program, GLint attributeLocation, bool normalized) 
{
	(void) program;
	SDL_assert(mTarget == GL_ARRAY_BUFFER);
	GLsizei typeSize = getTypeSize(mType);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
	glEnableVertexAttribArray(attributeLocation);		
	SDL_assert(glGetError() == GL_NO_ERROR);				
	glVertexAttribPointer(
		attributeLocation,						  	    /* attribute */
		mComponentCount,								/* component size */
		mType,											/* type */
		normalized ? GL_TRUE : GL_FALSE,				/* normalized? */
		typeSize * mComponentCount,						/* stride */
		(void*)0 );										/* array buffer offset */
	SDL_assert(glGetError() == GL_NO_ERROR);				
	return;
}

void Buffer::bindAttribute(Program* program, const std::string& attributeName, bool normalized) 
{
	GLint attributeLocation = program->getAttributeLocation(attributeName);
	bindAttribute(program, attributeLocation, normalized);
}

void Buffer::bindIndices() 
{
	SDL_assert(mTarget == GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

/**
 * Actually draw the contents of the buffer
 * @param mode Primitive type to draw eg GL_TRIANGLES
 */
void Buffer::draw(GLenum mode) const  {
	SDL_assert(mTarget == GL_ELEMENT_ARRAY_BUFFER);
	glDrawElements(mode, mSize, mType, (void*) 0);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

/**
 * Draw some of the buffer
 * @param mode Primitive to use
 * @param count Number of elements tor draw
 */
void Buffer::drawSome(GLenum mode, GLuint count) const
{
//	GLsizei typeSize = getTypeSize(mType);
	glDrawElements(mode, count, mType, (void*) 0);
}

void Buffer::drawImmediate(GLenum mode) const  
{
	SDL_assert(mTarget == GL_ARRAY_BUFFER);
	glDrawArrays(mode, 0, mSize);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Buffer::drawSomeImmediate(GLenum mode, GLuint count) const
{
//  GLsizei typeSize = getTypeSize(mType);
	glDrawArrays(mode, 0, count);
}

void Buffer::unbindIndices() 
{
	SDL_assert(mTarget == GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Buffer::unbindAttribute(Program* program, GLint attributeLocation) 
{
	(void) program;
	glDisableVertexAttribArray(attributeLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Buffer::unbindAttribute(Program* program, const std::string& attributeName) 
{
	GLint attributeLocation = program->getAttributeLocation(attributeName);
	unbindAttribute(program, attributeLocation);
}

Buffer::~Buffer() 
{
	glDeleteBuffers(1, &mBuffer);
}

}

