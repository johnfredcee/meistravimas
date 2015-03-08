#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus
extern "C" {
#endif

struct UIcontext;
struct NVGcontext;

#ifdef __cplusplus
}
#endif

	
namespace venk {

class Program;

class GuiService : public Service<GuiService> {
private:
	int guiWidth;
	int guiHeight;
	::UIcontext* uictx;
	::NVGcontext* vg;	
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

	void setCanvasDimensions(int width, int height) {
		guiWidth = width;
		guiHeight = height;
	}

	
	/* render the gui from the root, down */
	bool render();
	
protected:
	
	int keybObserver;
	int mausObserver;
	bool draw(float width, float height);
	bool mouse(SDL_Event* e);
	bool keyboard(SDL_Event* e);
};

}
#endif

