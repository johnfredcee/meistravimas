#include <iostream>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>

namespace venk
{

/**
 * Helper routine to show shader results
 * @param object GL object to show log of
 * @param glGet__iv Vector to glGet call
 * @param glGet__InfoLog vectror to glGetInfoLog call
 */

void show_info_log(
    GLuint object,
    PFNGLGETSHADERIVPROC glGet__iv,
    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
	GLint log_length;
	GLchar *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = (GLchar*) SDL_malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	std::cerr << log << std::endl;
	SDL_free(log);
}

GLenum show_gl_error(GLenum error)
{
	if (error == GL_NO_ERROR)
		return error;
	std::string msg;
	switch(error) {
	case GL_INVALID_OPERATION:
		msg="INVALID_OPERATION";
		break;
	case GL_INVALID_ENUM:
		msg="INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		msg="INVALID_VALUE";
		break;
	case GL_OUT_OF_MEMORY:
		msg="OUT_OF_MEMORY";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		msg="INVALID_FRAMEBUFFER_OPERATION";
		break;
	default:
		msg="UNKNOWN_ERROR";
		break;
	}
	std::cerr << "GL_" << msg.c_str() << std::endl;
	return error;
}

}
