
#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_opengl.h>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"

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
		mState.erase(name);
	}

	bool RenderStateService::exists(const std::string& name) const
	{
		return mState.find(name) != mState.end();
	}

}
