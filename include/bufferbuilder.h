#ifndef BUFFERBUILDER_H
#define BUFFERBUILDER_H

namespace venk
{

/**
 * Class used to build a binary buffer from a sequence of scalar tuples,
 * OpenGL retained-mode style.
 */
class BufferBuilder	{
private:
  /* pointer to actual buffer */
	Uint8  *mBuffer;
	/*  Number of scalars per element (eg 2 for 2f, 4 for 4ui, etc */
	GLsizei mComponentCount;
	/* portion of buffer occupied by data, in bytes */
	GLsizei mBufferSize;
	/* buffer total size in bytes */
	GLsizei mBufferCapacity;
	/* number of items held in buffer */
	GLsizei mItemCount;
	/* datatype of elements in the buffer */
	GLenum  mType;
public:	
	BufferBuilder(GLenum type, GLsizei componentCount);
	~BufferBuilder();
	/* add values to the buffer */
	void addVec1f(GLfloat x);
	void addVec2f(GLfloat x, GLfloat y);
	void addVec3f(GLfloat x, GLfloat y, GLfloat z);
	void addVec4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void addVec1ui(GLushort i);
	void addVec2ui(GLushort i, GLushort j);
	void addVec3ui(GLushort i, GLushort j, GLushort k);
	/* return a raw pointer to the data */
	const void *getData();
	/* return the number of items in the buffer */
	GLsizei itemCount();
private:
	void growBuffer();
	
};

}
#endif
