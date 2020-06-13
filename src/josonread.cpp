
#include <string>

#include <SDL.h>
#include <SDL_rwops.h>
#include <res_path.h>

#include "physfsrwops.h"
#include "picojson.h"
#include "jsonread.h"

namespace venk
{
	picojson::value read_json(const std::string &fileName)
	{
		picojson::value v;
		Uint64 fileSize; 
		void *data = get_resource_file("json", fileName.c_str(), &fileSize);
		if (fileSize > 0)
		{
			char *text = reinterpret_cast<char *>(data);
			std::string error_msg;
			picojson::parse(v, text, text + fileSize, &error_msg);
			SDL_assert(error_msg.empty());
			SDL_free(data);
		}
		return v;
	}
} // namespace venk
