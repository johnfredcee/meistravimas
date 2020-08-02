#ifndef TEXTEDIT_H
#define TEXTEDIT_H


namespace venk {

class TextEditService : public Service<TextEditService> {

private:
	::NVGcontext* vg;	

public:

	typedef TextEditService* ServicePtr_t;

	static bool initialise(TextEditService* self);

	static bool shutdown(TextEditService* self);

	::text_control* state;
};

}
#endif
