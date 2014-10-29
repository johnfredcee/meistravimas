#ifndef RES_PATH_H
#define RES_PATH_H


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
char *file_contents(const std::string& fileName, Sint32 *length);

}

#endif
