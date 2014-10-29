#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>


#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "shader.h"
#include "program.h"

namespace venk
{

size_t strnlen (const char *string, size_t maxlen)
{
	const char *end = (const char *) memchr (string, '\0', maxlen);
	return end ? (size_t) (end - string) : maxlen;
}

const char *ProgramService::computeName(const char *name)
{
    static char programName[Program::maxProgramNameLength];

    extern size_t strnlen(const char *string, size_t maxlen);

    int end = 0;
    while (isalnum(name[end]))
        end++;
    strncpy(programName, name, end);
    programName[end] = '\0';
    return programName;
}


bool ProgramService::initialise(ProgramService* self)
{
	// nothing to do
	(void) self;
	return true;
}

bool ProgramService::shutdown(ProgramService* self)
{
	// unload all programs: smart pointer should handle it
	self->programTable.clear();
	return true;
}

std::shared_ptr<Program> ProgramService::loadProgram(const std::string& name)
{
	std::string vshaderName(name);
	std::string fshaderName(name);
	ProgramService::ProgramTableEntry info;


	vshaderName += std::string("-v.glsl");
	fshaderName += std::string("-f.glsl");
	info.vShader = std::make_shared<Shader>(GL_VERTEX_SHADER,  vshaderName.c_str());
	SDL_assert(info.vShader);
	info.fShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fshaderName.c_str());
	SDL_assert(info.fShader);
	info.prog = std::make_shared<Program>(info.vShader, info.fShader);
	SDL_assert(info.prog);
	std::string progname(computeName(name.c_str()));
	std::cerr << "Created program named " << progname.c_str() << std::endl;
	programTable.insert(ProgramLookupTable_t::value_type(progname, info));
	return info.prog;
}

bool ProgramService::programExists(const std::string& name) const
{
	return (programTable.find(name) != programTable.end());
}

std::shared_ptr<Program> ProgramService::getProgram(const std::string& name) const
{
	return (*programTable.find(name)).second.prog;
}


/* (Possible scheme to use .. not yet used in this code
   Index    Built-in    Custom Name
   0     gl_Vertex  vertex
   1    n/a blendWeights
   2    gl_Normal   normal
   3    gl_Color    colour
   4    gl_SecondaryColor   secondary_colour
   5    gl_FogCoord n/a
   7    n/a blendIndices
   8    gl_MultiTexCoord0   uv0
   9    gl_MultiTexCoord1   uv1
   10   gl_MultiTexCoord2   uv2
   11   gl_MultiTexCoord3   uv3
   12   gl_MultiTexCoord4   uv4
   13   gl_MultiTexCoord5   uv5
   14   gl_MultiTexCoord6   uv6, tangent
   15   gl_MultiTexCoord7   uv7, binormal
*/

Program::Program(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader) : programOk(0), program(0)
{
	extern void show_info_log(GLuint object,
	                          PFNGLGETSHADERIVPROC glGet__iv,
	                          PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);


	SDL_assert(vertexShader && fragmentShader);
	if (vertexShader && fragmentShader) {
		program = glCreateProgram();

		glAttachShader(program, vertexShader->shader);
		SDL_assert(glGetError() == GL_NO_ERROR);
		glAttachShader(program, fragmentShader->shader);
		SDL_assert(glGetError() == GL_NO_ERROR);

		/* POSSIBLE BINDING SCHEME
		   glBindAttribLocation(program, 0, "vertex");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 1, "blendWeights");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 2, "normal");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 3, "color");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 4, "secondary_colour");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 7, "blendIndices");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 8, "uv0");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 9, "uv1");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 10, "uv2");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 11, "uv3");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 12, "uv3");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 13, "uv3");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 14, "tangent");
		   assert(glGetError() == GL_NO_ERROR);
		   glBindAttribLocation(program, 15, "binormal");
		   assert(glGetError() == GL_NO_ERROR);
		*/

		glLinkProgram(program);
		SDL_assert(glGetError() == GL_NO_ERROR);
		glGetProgramiv(program, GL_LINK_STATUS, &programOk);
		show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		if (programOk != GL_TRUE) {
			std::cerr << "Failed to link shader program." << std::endl;
			glDeleteProgram(program);
		} else {
			scanAttributes();
			scanUniforms();
		}
		SDL_assert(glGetError() == GL_NO_ERROR);
		SDL_assert(glIsProgram(program)  == GL_TRUE);
		return;
	}
}

void Program::scanAttributes()
{
	// scan attributes
	Sint32 param;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &param);
	SDL_assert(glGetError() == GL_NO_ERROR);
	Sint32 attributeCount = param;
	SDL_assert(attributeCount < maxAttributes);
	std::cerr << attributeCount << " active attributes\n" << std::endl;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &param);
	SDL_assert(glGetError() == GL_NO_ERROR);
	Sint32  bufLen = param + 1;
	char *buffer = new char[bufLen];
	memset(buffer, 0, bufLen);
	for (Sint32 i = 0; i < attributeCount; ++i) {
		Attribute a;
		Sint32 size;
		glGetActiveAttrib(program, i, bufLen, NULL, &size, &a.type, buffer);
		a.location = glGetAttribLocation(program, buffer);
		std::cerr << "Attribute " << i << " @ " << std::hex << a.location << " named " << buffer << " size " << std::dec << size << " type " << a.type << std::endl;
		std::string attributeName(buffer);
		attributes[attributeName] = a;
	}
	delete [] buffer;
}

void Program::scanUniforms()
{
	// scan uniforms
	Sint32 param;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &param);
	SDL_assert(glGetError() == GL_NO_ERROR);
	Sint32 uniformCount = param;
	std::cerr << uniformCount << " active uniforms" << std::endl;
	SDL_assert(uniformCount < maxUniforms);
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &param);
	SDL_assert(glGetError() == GL_NO_ERROR);
	Sint32 bufLen = param + 1;
	char *buffer = new char[bufLen];
	memset(buffer, 0, bufLen);
	for (Sint32 i = 0; i < uniformCount; ++i) {
		Uniform u;
		Sint32 size;
		glGetActiveUniform(program, i, bufLen, NULL, &size, &u.type, buffer);
		u.location = glGetUniformLocation(program, buffer);
		std::cerr << "Uniform " << i << " @ " << std::hex << u.location << " named " << buffer << " size " << std::dec << size << " type " << std::hex << u.type << std::endl;
		std::string uniformName(buffer);
		uniforms[uniformName] =  u;
	}
	delete [] buffer;
}

bool Program::isValid() const
{
	return ((program != 0) && (programOk != 0) && (glIsProgram(program) == GL_TRUE));
}

void Program::use() const
{
	extern GLenum show_gl_error(GLenum error);
	SDL_assert(glIsProgram(program)  == GL_TRUE);
	glUseProgram(program);
	SDL_assert(show_gl_error(glGetError()) == GL_NO_ERROR);
}

void Program::unUse() const
{
	extern GLenum show_gl_error(GLenum error);
	glUseProgram(0);
	SDL_assert(show_gl_error(glGetError()) == GL_NO_ERROR);
}

bool Program::inUse() const
{
	Sint32 used;
	glGetIntegerv(GL_CURRENT_PROGRAM, &used);
	return ((GLuint) used == program);
}

Sint32 Program::getUniformLocation(const std::string& name) const
{
	auto it = uniforms.find(name);
	SDL_assert(it != uniforms.end());
	return it->second.location;
}

Sint32 Program::operator[](const std::string& name) const
{
	auto it = uniforms.find(name);
	SDL_assert(it != uniforms.end());
	return it->second.location;
}

void Program::setUniform1f(Sint32 location, const GLfloat *vec) 
{
	glUniform1fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform1f(const std::string& name, const GLfloat* vec) 
{
	Sint32 location = getUniformLocation(name);
	glUniform1fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform2f(Sint32 location, const GLfloat *vec)
{
	glUniform2fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform2f(const std::string& name, const GLfloat* vec)
{
	Sint32 location = getUniformLocation(name);
	glUniform2fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform3f(Sint32 location, const GLfloat *vec)
{
	glUniform3fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform3f(const std::string& name, const GLfloat* vec)
{
	Sint32 location = getUniformLocation(name);
	glUniform3fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform4f(Sint32 location, const GLfloat *vec)
{
	glUniform4fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniform4f(const std::string& name, const GLfloat* vec)
{
	Sint32 location = getUniformLocation(name);
	glUniform4fv(location, 1, vec);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniformMat3f(Sint32 location, const GLfloat *mat3)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, mat3);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniformMat3f(const std::string& name, const GLfloat* mat3)
{
	Sint32 location = getUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, mat3);
	SDL_assert(glGetError() == GL_NO_ERROR);
}


void Program::setUniformMat4f(Sint32 location, const GLfloat *mat4)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, mat4);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniformMat4f(const std::string& name, const GLfloat* mat4)
{
	Sint32 location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, mat4);
	SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniformSampler(Sint32 location, Sint32 textureUnit, Texture *texture)
{
	// SDL_assert(texture);
	// glActiveTexture(GL_TEXTURE0 + textureUnit);
	// texture->select();
	// glUniform1i(location, textureUnit);
	// SDL_assert(glGetError() == GL_NO_ERROR);
}

void Program::setUniformSampler(const std::string& name, Sint32 textureUnit, Texture *texture)
{
	// SDL_assert(texture);
	// glActiveTexture(GL_TEXTURE0 + textureUnit);
	// texture->select();
	// glUniform1i(location, textureUnit);
	// SDL_assert(glGetError() == GL_NO_ERROR);
}

Sint32 Program::getAttributeLocation(const std::string& name)
{
	AttributeDictionaryT::iterator it = attributes.find(std::string(name));
	SDL_assert(it != attributes.end());
	return it->second.location;
}

Sint32 Program::getAttributeCount() const
{
	return attributes.size();
}

Program::~Program()
{
	if (program != 0)
		glDeleteProgram(program);
	program = 0;
	programOk = 0;
}

}
