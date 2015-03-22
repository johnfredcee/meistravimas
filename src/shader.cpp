
#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <res_path.h>
#include "utils.h"
#include "shader.h"

using namespace venk;


Shader::Shader(GLenum type, const std::string& filename) :  shaderOk(false), shader(0)
{

	extern void show_info_log(GLuint object,
	                          PFNGLGETSHADERIVPROC glGet__iv,
	                          PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);

	Uint64 length = 0;
	char* source = file_contents(filename.c_str(), &length);
	SDL_assert_always((source != nullptr) && (length > 0));
	if ((!source) || (length == 0)) {
		std::cerr << "Could not load " << filename << std::endl;
		return;
	}

	shader = glCreateShader(type);
	Sint32 shader_length = static_cast<Sint32>(length);
	glShaderSource(shader, 1, (const GLchar**)&source, &shader_length);
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
