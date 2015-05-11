#ifndef PROGRAM_H
#define PROGRAM_H

namespace venk
{

class Shader;
class Texture;
class Program;

class ProgramService : public Service<ProgramService>
{
public:

    typedef ProgramService *ServicePtr_t;

    /**
     * Initialse the program subsystem
     * @return true if successful
     */
    static bool initialise(ProgramService* self);

    /**
     * Shutdown the program subsystem.
     * @return returns true if successful
     */
    static bool shutdown(ProgramService* self);

    /**
     * Actually create a program via the service
     */
    std::shared_ptr<Program> loadProgram(const std::string& name);

    /**
     * Load a program by name..
     */
    std::shared_ptr<Program> getProgram(const std::string& name) const;

	/**
     * return the name of the program currently in use
     */
	std::string currentProgram() const;
	
    /**
     * check to see if a program exists
     */
    bool programExists(const std::string& name) const;

private:

    struct ProgramTableEntry
    {
        std::shared_ptr<Program>  prog;
        std::shared_ptr<Shader>   vShader;
        std::shared_ptr<Shader>   fShader;
    };

    typedef std::unordered_map<std::string, ProgramTableEntry>  ProgramLookupTable_t;
    typedef std::unordered_map<Sint32, std::string >    ProgramIndexTable_t;

    const char *computeName(const char *name);

    ProgramLookupTable_t programTable;
	ProgramIndexTable_t  programIndex;
};

class Program
{
	friend class ProgramService;
	
public:
    static const int maxUniforms   = 64;
    static const int maxAttributes = 16;
    static const int maxProgramNameLength = 63;

    struct Uniform
    {
        Sint32  location;
        GLenum type;
    };

    struct Attribute
    {
        Sint32  location;
        GLenum type;
    };

private:
    Sint32  programOk;
    GLuint program;

    typedef std::unordered_map<std::string, Uniform>     UniformDictionaryT;
    typedef std::unordered_map<std::string, Attribute> AttributeDictionaryT;

    UniformDictionaryT   uniforms;
    AttributeDictionaryT attributes;
    void scanUniforms();
    void scanAttributes();

public:
    Program(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);
    void  use()   const;
    void  unUse() const;
    bool  inUse() const;

    // attribute related
    Sint32 getAttributeCount() const;
    Sint32 getAttributeLocation(const std::string& name);
	void getAttributeList(std::vector<std::string>& uniforms);

    // uniform related
    Sint32 getUniformLocation(const std::string& name) const;
	Sint32 operator[](const std::string& name) const;
	void getUniformList(std::vector<std::string>& uniform_list);
	
	void  setUniform1f(const std::string& name, const GLfloat *vec);
    void  setUniform2f(const std::string& name, const GLfloat *vec);
    void  setUniform3f(const std::string& name, const GLfloat *vec);
    void  setUniform4f(const std::string& name, const GLfloat *vec);
    void  setUniformMat3f(const std::string& name, const GLfloat *mat3);
    void  setUniformMat4f(const std::string& name, const GLfloat *mat4);
    void  setUniformSampler(const std::string& name, Sint32 textureUnit, Texture *texture);
	
    void  setUniform1f(Sint32 location, const GLfloat *vec);
    void  setUniform2f(Sint32 location, const GLfloat *vec);
    void  setUniform3f(Sint32 location, const GLfloat *vec);
    void  setUniform4f(Sint32 location, const GLfloat *vec);
    void  setUniformMat3f(Sint32 location, const GLfloat *mat3);
    void  setUniformMat4f(Sint32 location, const GLfloat *mat4);
    void  setUniformSampler(Sint32 location, Sint32 textureUnit, Texture *texture);
    virtual ~Program();
    bool isValid() const;
};

}
#endif
