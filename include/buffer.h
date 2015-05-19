#ifndef BUFFER_H
#define BUFFER_H

namespace venk
{
class Program;
class Buffer;

class BufferManagerService : public Service<BufferManagerService>
{
	friend class Buffer;
public:
	typedef BufferManagerService *ServicePtr_t;

	/**
	 * Initialse the buffer  subsystem
	 * @return true if successful
	 */
	static bool initialise(BufferManagerService* self);
	
	/**
	 * Shutdown the buffer subsystem.
	 * @return returns true if successful
	 */
	static bool shutdown(BufferManagerService* self);

	/** Actually create a buffer using the service */
	std::shared_ptr<Buffer> make_buffer(GLenum		target,
										const void  *bufferData,
										GLenum		type,
										GLsizei		count,
										GLsizei		componentCount = 1,
										GLenum		usage = GL_STATIC_DRAW);

};


class Buffer {
private:
	GLuint	mBuffer;
	GLuint	mTarget;
	GLsizei mSize;
	GLsizei mComponentCount;
	GLenum	mType;

public:

	Buffer(GLenum		target,
		   const void  *bufferData,
		   GLenum	type,
		   GLsizei		count,
		   GLsizei		componentCount = 1,
		   GLenum		usage = GL_STATIC_DRAW);
	~Buffer();
	GLuint getSize() const {
		return mSize;
	}
	GLenum getType() const {
		return mType;
	}
    void update(const void *bufferData );
	void bindAttribute(Program* program, GLint attributeIndex, bool normalized = false);
	void bindAttribute(Program* program, const std::string& attributeName, bool normalized = false);
	void bindIndices();
	void draw(GLenum mode = GL_TRIANGLE_STRIP) const;
	void drawSome(GLenum mode, GLuint count) const;
	void drawRange(GLenum mode, GLuint offset, GLuint count) const;
	void drawImmediate(GLenum mode = GL_TRIANGLE_STRIP) const;
	void drawSomeImmediate(GLenum mode, GLuint count) const;
	void unbindIndices();
	void unbindAttribute(Program* program, GLint attributeIndex);
	void unbindAttribute(Program* program, const std::string& attributeName);
};

template <typename T> 
struct BufferInitializer
{
	static std::shared_ptr<Buffer> MakeFloatBuffer(const T& bufferData, GLenum usage = GL_STATIC_DRAW) {
		return std::make_shared<Buffer>(GL_ARRAY_BUFFER, (const void*) &bufferData[0], GL_FLOAT, bufferData.size(), T::value_type::value_size(), usage);
	}
	static std::shared_ptr<Buffer> MakeElementBuffer(const T& bufferData, GLenum usage = GL_STATIC_DRAW) {
		return std::make_shared<Buffer>(GL_ELEMENT_ARRAY_BUFFER, &bufferData[0], GL_UNSIGNED_SHORT, bufferData.size(), 1, usage);
	}
};

}

#endif
