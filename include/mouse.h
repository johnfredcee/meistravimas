#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#define MAXIMUM_MOUSE_BUTTONS (8)
#define MOUSE_BUTTON_PRESSED  (SDL_PRESSED)
#define MOUSE_BUTTON_RELEASED (SDL_RELEASED)

namespace venk
{
	
class Mouse  {

	friend class MouseService;
	
public:
	int x;
	int y;
	float dx;
	float dy;
	int wheel;
	double sampleTime;
	int buttons[MAXIMUM_MOUSE_BUTTONS];

private:
	bool firstsample;

    Mouse() {};
    ~Mouse() {};
};

class MouseService : public Service< MouseService >
{

public:
	typedef MouseService* ServicePtr_t;

	Observable<MouseService> observers;
	
	/**
	 * Initialise the murine subsystem
	 * @return true if successful
	 */
	static bool initialise(MouseService* self);


	/** 
     * Shutdown the mouse subsystem
     * @return true if successful
     */
	static bool shutdown(MouseService* self);

	/**
	 * Check for mouse motion events and things..
	 */
	void sample();

	const Mouse& mouse();

private:
    /** The mouse herself. */
	Mouse *state;
};


}
#endif
