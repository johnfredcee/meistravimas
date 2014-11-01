#ifndef TINYSCM_IF_H
#define TINYSCM_IF_H

namespace venk
{
	void load_scheme(scheme* sc, const std::string& fname);
	int server(scheme *sc);

}

#endif
