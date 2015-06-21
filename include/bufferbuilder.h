#ifndef BUFFERBUILDER_H
#define BUFFERBUILDER_H

namespace venk
{
	class Buffer;
	
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
	BufferBuilder(GLenum type, GLsizei componentCount, Uint32 elementCount = 32);
	~BufferBuilder();
	/* add values to the buffer */
	void addVec1f(GLfloat x);
	void setVec1f(Uint32 i, GLfloat x);
	// v2
	void addVec2f(GLfloat x, GLfloat y);
	void addVec2f(const Vector2d& v);
	void setVec2f(Uint32 i, GLfloat x, GLfloat y);	
	void setVec2f(Uint32 i, const Vector2d& v);
	// v3
	void addVec3f(GLfloat x, GLfloat y, GLfloat z);
	void addVec3f(const Vector3d& v);
	void setVec3f(Uint32 i, GLfloat x, GLfloat y, GLfloat z);
	void setVec3f(Uint32 i, const Vector3d& v);
	// v4
	void addVec4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void setVec4f(Uint32 i, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	// vi
	void addVec1ui(GLushort i);
	void addVec2ui(GLushort i, GLushort j);
	void addVec3ui(GLushort i, GLushort j, GLushort k);
	/* return a raw pointer to the data */
	const void *getData();
	/* return the number of items in the buffer */
	GLsizei itemCount();
	/* return the type of the buffer (type and component count) */
	void getTypeInfo(GLenum& type, GLsizei& component_count);
	std::shared_ptr<Buffer> make_buffer(GLenum target, GLenum = GL_STATIC_DRAW);
	void update_buffer(std::shared_ptr<Buffer> buffer);
private:
	void growBuffer();
	
};

}
#endif
