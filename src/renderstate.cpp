
#include <SDL.h>
#include <SDL_opengl.h>

#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "tuple.h"
#include "threedee.h"
#include "quat.h"
#include "matrix44.h"
#include "shader.h"
#include "program.h"
#include "renderstate.h"

namespace venk
{

	bool RenderStateService::initialise(RenderStateService* self)
	{
		self->mState.reserve(256);		
		return true;
	}

	bool RenderStateService::shutdown(RenderStateService* self)
	{
		(void) self;
		return true;
	}

	void RenderStateService::set(const std::string& name, const RenderParameter& param)
	{
		auto it = mState.find(name);
		if (it == mState.end()) {
			mState.emplace(name, param);
		} else {
			mState.insert(it, std::make_pair(name, param));
		}
	}

	const RenderParameter& RenderStateService::operator[](const std::string& name) const
	{
		auto it = mState.find(name);
		SDL_assert(it != mState.end());
		return it->second;
	}

	void RenderStateService::remove(const std::string& name)
	{
		SDL_assert(exists(name));
		mState.erase(name);
	}

	bool RenderStateService::exists(const std::string& name) const
	{
		return mState.find(name) != mState.end();
	}

	/** bind to uniform of program */
	void RenderStateService::uniform(const std::string& name, Program* program, Sint32 uniform)
	{
		SDL_assert(exists(name));
		auto it = mState.find(name);
		switch (it->second.type)
		{
		case eFLOAT:
			program->setUniform1f(uniform, &(it->second.value.f));
			break;
		case eFLOAT_VEC2:
			program->setUniform2f(uniform, &(it->second.value.v2[0]));
			break;
		case eFLOAT_VEC3:
			program->setUniform3f(uniform, &(it->second.value.v3[0]));
			break;
		case eFLOAT_VEC4:
			program->setUniform4f(uniform, &(it->second.value.v4[0]));
			break;
		case eMAT3:
			program->setUniformMat3f(uniform, &(it->second.value.mat3[0]));
			break;
		case eMAT4:
			program->setUniformMat4f(uniform, &(it->second.value.mat4[0]));
			break;				
		}
	}

	/** bind to uniform of program */
	void RenderStateService::uniform(const std::string& name, Program* program, const std::string& uniform)
	{
		Sint32 loc = program->getUniformLocation(uniform);
		this->uniform(name, program, loc);
	}

}
