
#ifndef RENDER_STATE_H_INCLUDED
#define RENDER_STATE_H_INCLUDED

namespace venk
{

class Program;

enum eRenderParameterType
{
	eFLOAT,
	eFLOAT_VEC2,
	eFLOAT_VEC3,
	eFLOAT_VEC4,
	eMAT3,
	eMAT4
};

union RenderParameterValue
{
	float f;
	float v2[2];
	float v3[3];
	float v4[4];
	float mat3[9];
	float mat4[16];
};

// to do -- parameter computed from other values, no explicit value? fn type param
class RenderParameter
{
public:
	eRenderParameterType type;
	RenderParameterValue value;

	RenderParameter(const Matrix44& mat)
	{
		type = eMAT4;
		for(Uint32 i = 0; i < 16; i++) {
			value.mat4[i] = mat.elements[i];
		}
		return;
	}


	RenderParameter& operator=(const Matrix44& mat)
	{
		type = eMAT4;
		for(Uint32 i = 0; i < 16; i++) {
			value.mat4[i] = mat.elements[i];
		}
		return *this;
	}

	RenderParameter(const Vector3d& mat)
	{
		this->type = eFLOAT_VEC3;
		for(Uint32 i = 0; i < 3; i++) {
			this->value.v3[i] = mat.elements[i];
		}
		return;
	}

	RenderParameter& operator=(const Vector3d& mat)
	{
		this->type = eFLOAT_VEC3;
		for(Uint32 i = 0; i < 3; i++) {
			this->value.v3[i] = mat.elements[i];
		}
		return *this;
	}

};


class RenderStateService : public Service<RenderStateService>
{
public:
	typedef RenderStateService *ServicePtr_t;

    typedef std::unordered_map<std::string, RenderParameter> RenderParameterDictionaryT;
private:
	RenderParameterDictionaryT mState;

public:
    /**
     * Initialse the image subsystem
     * @return true if successful
     */
    static bool initialise(RenderStateService* self);

    /**
     * Shutdown the image subsystem.
     * @return returns true if successful
     */
    static bool shutdown(RenderStateService* self);

	/** Access a render state parameter */
	const RenderParameter& operator[](const std::string& name) const;

	/** Set the value of a new render state parameter */
	void set(const std::string& name, const RenderParameter& param);

	/** remove an existing render state parameter */
	void remove(const std::string& name);

	/** test to see if a renderstate parameter exists */
	bool exists(const std::string& name) const;

	/** bind to uniform of program */
	void uniform(const std::string& name, Program* program, Sint32 location);

	/** bind to uniform of program */
	void uniform(const std::string& name, Program* program, const std::string& uniform);
};

}
#endif
