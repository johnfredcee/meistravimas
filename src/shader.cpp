
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <res_path.h>
#include "shader.h"

using namespace venk;

char *file_contents(const char *fileName, Sint32 *length)
{
	std::string fullFileName = getResourcePath();
	fullFileName = fullFileName + fileName;
	SDL_RWops *rwops = SDL_RWFromFile(fullFileName.c_str(), "rb");
	if (rwops != nullptr) {
		Sint64 size = SDL_RWsize(rwops);
		if (size != -1L) {
			void* buf = SDL_malloc(size+1);
			size_t read = SDL_RWread(rwops, buf, 1, size);
			if (read == size) {
				*(((char*)buf) + size) = '\0';
				*length = static_cast<Sint32>(size);
				return (char*) buf;
			} else {
				std::cerr << "Failed to read " << fullFileName << std::endl;
			}
		} else {
			std::cerr << "Failed to read " << fullFileName << std::endl;
		}
	} else {
		std::cerr << "Failed to open " << fullFileName << std::endl;
	}
	return (GLchar*) nullptr;
}

Shader::Shader(GLenum type, const std::string& filename) :  shaderOk(false), shader(0)
{

	extern void show_info_log(GLuint object,
	                          PFNGLGETSHADERIVPROC glGet__iv,
	                          PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);

	Sint32 length = 0;
	char* source = file_contents(filename.c_str(), &length);

	if ((!source) || (length == 0)) {
		std::cerr << "Could not load " << filename << std::endl;
		return;
	}

	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &length);
	glCompileShader(shader);
	SDL_free(source);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderOk);
	if (!shaderOk) {
		std::cerr << "Failed to compile " << filename << std::endl;
		show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shader);
		shader = 0;
	}
	return;
};

bool Shader::isValid() const
{
	return (shader != 0) && (shaderOk != 0);
}

Shader::~Shader()
{
	glDeleteShader(shader);
	shader = 0;
	shaderOk = 0;
}
