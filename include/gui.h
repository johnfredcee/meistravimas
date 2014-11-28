#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus

namespace venk {

class Program;

class GuiService : public Service<GuiService> {
private:
	Matrix44 guiTransform;
	std::shared_ptr<Program> gui;
	static std::shared_ptr<Panel> root;
public:
	typedef GuiService *ServicePtr_t;

	/**
	 * Initialse the gui subsystem
	 * @return true if successful
	 */
	static bool initialise(GuiService* self);

	/**
	 * Shutdown the gui subsystem.
	 * @return returns true if successful
	 */
	static bool shutdown(GuiService* self);

	std::shared_ptr<Program> program() { return gui; };

	/* render the gui from the root, down */
	bool render();
};

}
#endif

#endif
