#ifndef KEYS_H
#define KEYS_H

class Keyboard;

#define MAXIMUM_KEYBOARD_KEYS (512)

#define KEY_PRESS 1
#define KEY_RELEASE 0

#define ESC_KEY SDL_SCANCODE_ESCAPE

namespace venk
{

class Keyboard
{

    friend class KeyboardService;

  public:
    int keys[MAXIMUM_KEYBOARD_KEYS];
};

class KeyboardService : public Service<KeyboardService>
{
  public:
    typedef KeyboardService *ServicePtr_t;

    Observable<KeyboardService> observers;

    /**
		* Initialise the keyboard subsystem
		* @return true if successful
		*/
    static bool initialise(KeyboardService *self);

    /** 
		* Shutdown the keyboard subsystem
		* @return true if successful
		*/
    static bool shutdown(KeyboardService *self);

    void sample();

    const Keyboard &keyboard();

  private:
    /** The keyboard herself. */
    Keyboard *state;
};

}
#endif
