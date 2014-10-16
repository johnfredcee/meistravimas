
#include <glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_assert.h>
#include "utils.h"

GLsizei getTypeSize(GLenum type)  {

	GLsizei typeSize = 0;

	switch (type) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			typeSize = sizeof(GLfloat);
			break;
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
			typeSize = sizeof(GLubyte);
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			typeSize = sizeof(GLushort);
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
			typeSize = sizeof(GLuint);
			break;
		default:
			break;
	};

	SDL_assert(typeSize != 0);
	return typeSize;

}

GLint getTypeComponentCount(GLenum type)
{
	GLint result;
	switch (type) {
		case GL_FLOAT:
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
		case GL_UNSIGNED_INT:
		case GL_INT:
			result = 1;
			break;
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
			result = 2;
			break;
		case GL_FLOAT_VEC3:
		case GL_INT_VEC3:
			result = 3;
			break;
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
			result = 4;
			break;
		case GL_FLOAT_MAT2:
			result = 4;
			break;
		case GL_FLOAT_MAT3:
			result = 9;
			break;
		case GL_FLOAT_MAT4:
			result = 16;
			break;
		case GL_FLOAT_MAT2x3:
			result = 2*3;
			break;
		case GL_FLOAT_MAT2x4:
			result = 2*4;
			break;
		case GL_FLOAT_MAT3x2:
			result = 3*2;
			break;
		case GL_FLOAT_MAT3x4:
			result = 3*4;
			break;
		case GL_FLOAT_MAT4x2:
			result = 4*2;
			break;
		case GL_FLOAT_MAT4x3:
			result = 4*3;
			break;
		case GL_SAMPLER_1D:
			result = -1;
			break;
		case GL_SAMPLER_2D:
			result = -2;
			break;
		case GL_SAMPLER_3D:
			result = -3;
			break;
			/*	GL_SAMPLER_CUBE, GL_SAMPLER_1D_SHADOW, or GL_SAMPLER_2D_SHADOW	*/	   
		default:
			result = 0;
			break;
	}
	return result;
}

char *getTypeDescription(GLenum type)
{
	char *result;
	switch (type) {
		case GL_FLOAT:
			result = " GL_FLOAT";
			break;
		case GL_FLOAT_VEC2:
			result = " GL_FLOAT_VEC2";
			break;
		case GL_FLOAT_VEC3:
			result = " GL_FLOAT_VEC3";
			break;
		case GL_FLOAT_VEC4:
			result = " GL_FLOAT_VEC4";
			break;
		case GL_FLOAT_MAT2:
			result = " GL_FLOAT_MAT2";
			break;
		case GL_FLOAT_MAT3:
			result = " GL_FLOAT_MAT3";
			break;
		case GL_FLOAT_MAT4:
			result = " GL_FLOAT_MAT4";
			break;
		case GL_FLOAT_MAT2x3:
			result = " GL_FLOAT_MAT2x3";
			break;
		case GL_FLOAT_MAT2x4:
			result = " GL_FLOAT_MAT2x4";
			break;
		case GL_FLOAT_MAT3x2:
			result = " GL_FLOAT_MAT3x2";
			break;
		case GL_FLOAT_MAT3x4:
			result = " GL_FLOAT_MAT3x4";
			break;
		case GL_FLOAT_MAT4x2:
			result = " GL_FLOAT_MAT4x2";
			break;
		case GL_FLOAT_MAT4x3:
			result = " GL_FLOAT_MAT4x3";
			break;
		case GL_UNSIGNED_BYTE:
			result = " GL_UNSIGNED_BYTE";
			break;
		case GL_BYTE:
			result = " GL_BYTE";
			break;
		case GL_UNSIGNED_SHORT:
			result = " GL_UNSIGNED_SHORT";
			break;
		case GL_SHORT:
			result = " GL_SHORT";
			break;
		case GL_UNSIGNED_INT:
			result = " GL_UNSIGNED_INT";
			break;
		case GL_INT:
			result = " GL_INT";
			break;
		case GL_INT_VEC2:
			result = " GL_INT_VEC2";
			break;
		case GL_INT_VEC3:
			result = " GL_INT_VEC3";
			break;
		case GL_INT_VEC4:
			result = " GL_INT_VEC4";
			break;
		case GL_SAMPLER_1D:
			result = " GL_SAMPLER_1D";
			break;
		case GL_SAMPLER_2D:
			result = " GL_SAMPLER_2D";
			break;
		case GL_SAMPLER_3D:
			result = " GL_SAMPLER_3D";
			break;
		case GL_SAMPLER_CUBE:
			result = " GL_SAMPLER_CUBE";
			break;
		case GL_SAMPLER_1D_SHADOW:
			result = " GL_SAMPLER_1D_SHADOW";
			break;
		case GL_SAMPLER_2D_SHADOW:
			result = " GL_SAMPLER_2D_SHADO";
			break;
		default:
			result = " UNKNOWN";
	}
	return result;
}
