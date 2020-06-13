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

	// TODO - this is the C++ interface; probably should return a smart ptr
	char *file_contents(const std::string &fileName, Uint64 *length);

} // namespace venk
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	void *get_resource_file(const char *type, const char *name, Uint64 *length);
	SDL_RWops *open_resource(const char *type, const char *name);
	void close_resource(SDL_RWops *resource);
#ifdef __cplusplus
}
#endif

#endif
