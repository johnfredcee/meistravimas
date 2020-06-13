
#include <iostream>
#include <string>
#include <physfs.h>
#include <physfsrwops.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <res_path.h>

namespace venk {

/*
 * Get the resource path for resources located in res/subDir
 */
std::string getResourcePath(const std::string &subDir) {
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	//This will hold the base resource path.
	//We give it static lifetime so that we'll only need to call
	//SDL_GetBasePath once to get the executable path
	static std::string baseRes;
	if(baseRes.empty()) {
		//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
		char *basePath = SDL_GetBasePath();
		if(basePath) {
			baseRes = basePath;
			SDL_free(basePath);
		} else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//We replace the last build/ with res/ to get the the resource path
		size_t pos = baseRes.rfind("build");
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
	}
	//If we want a specific subdirectory path in the resource directory
	//append it to the base path.
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

char *file_contents(const std::string& fileName, Uint64 *length) {
	SDL_RWops *rwops = PHYSFSRWOPS_openRead(fileName.c_str());
	if(rwops != nullptr) {
		Sint64 size = SDL_RWsize(rwops);
		if(size != -1L) {
			void* buf = SDL_malloc(size+1);
			size_t read = SDL_RWread(rwops, buf, 1, size);
			SDL_RWclose(rwops);
			if(read == (std::size_t) size) {
				*(((char*)buf) + size) = '\0';
				*length = static_cast<Sint32>(size);
				return (char*) buf;
			} else {
				std::cerr << "Failed to read " << fileName << std::endl;
			}
		} else {
			std::cerr << "Failed to read " << fileName << std::endl;
		}
	} else {
		std::cerr << "Failed to open " << fileName << std::endl;
	}
	return (GLchar*) nullptr;
}

}

// C binding
extern "C"
{
	void* get_resource_file(const char* type, const char* name, Uint64 *length) {
		std::string path(type);
		path += std::string("/");
		path += std::string(name);
		return reinterpret_cast<void*>(venk::file_contents(path, length));
	}

	SDL_RWops* open_resource(const char* type, const char* name) {
		SDL_RWops* result = NULL;
		std::string path(type);
		path += std::string("/");
		path += std::string(name);
		result = PHYSFSRWOPS_openRead(path.c_str());
		SDL_assert(result != NULL);
		return result;
	}

	void close_resource(SDL_RWops* resource) {
		SDL_RWclose(resource);
	}
}




