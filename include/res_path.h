#ifndef RES_PATH_H
#define RES_PATH_H

#ifdef __cplusplus
namespace venk
{

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *
 */
std::string getResourcePath(const std::string &subDir = "");
char *file_contents(const std::string& fileName, Uint64 *length);

}
#endif

extern "C"{
	void *get_resource_file(const char *type, const char* name, Uint64 *length);
}

#endif
