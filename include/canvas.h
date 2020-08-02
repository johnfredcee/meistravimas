#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

struct NVGcontext;

extern NVGcontext* get_canvas_context();

#ifdef __cplusplus
}
#endif

	
namespace venk {

class Program;

class CanvasService : public Service<CanvasService> {
	friend class FontService;
	friend class TextEditService;
	friend ::NVGcontext* get_canvas_context();
private:
	::NVGcontext* vg;	
public:
	typedef CanvasService *ServicePtr_t;

	/**
	 * Initialse the gui subsystem
	 * @return true if successful
	 */
	static bool initialise(CanvasService* self);

	/**
	 * Shutdown the gui subsystem.
	 * @return returns true if successful
	 */
	static bool shutdown(CanvasService* self);

	
	/* begin rendering (for context switching) */
	void begin_frame(int width, int height);
	void end_frame();

	/* render the whole canvas */
	bool render(int width = -1, int height = -1);
	
};

}
#endif

