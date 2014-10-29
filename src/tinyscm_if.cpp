
#include <stding>
#include <physfs.h>
#include <scheme-private.h>
#include <SDL.h>
#include <SDL_rwops.h>

namespace venk
{

void load_scheme(scheme* sc, const std::string& fname)
{	
	std::string realfname = std::string("scheme/") +fname + std::string(".scm");
	Sint32 length;
	char* source = file_contents(realfname.c_str(), &length);
	SDL_assert_always((source != nullptr) && (length > 0));
	if ((!source) || (length == 0)) {
		std::cerr << "Could not load " << filename << std::endl;
		return;
	}
	scheme_load_string(sc, source);
	return;
}


}
