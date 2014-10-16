#ifndef SHADER_H
#define SHADER_H

namespace venk
{
	class Program;

	class Shader {
		friend class Program;
	private:
		Sint32  shaderOk;
		Uint32  shader;
	public:
		Shader(GLenum type, const std::string& filename);
		~Shader();
		bool isValid() const;
	};
}

#endif
