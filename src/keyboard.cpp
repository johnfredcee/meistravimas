#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "observer.h"
#include "keyboard.h"

namespace venk
{

bool KeyboardService::initialise(KeyboardService *self) {

	if (self != nullptr) {
		self->state = new Keyboard();
		for(int i = 0; i < MAXIMUM_KEYBOARD_KEYS; ++i) {
			self->state->keys[i] = KEY_RELEASE;
		}
	}
	return true;
}

bool KeyboardService::shutdown(KeyboardService *self) {
	if (self != nullptr) {
		delete self->state;
	}	
	return true;
}

void KeyboardService::sample()
{
 SDL_Event			event;
  
  while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP) != 0) {
	  SDL_KeyboardEvent *keyEvent = (SDL_KeyboardEvent*)(&event);
	  switch (event.type) {
		  case SDL_KEYUP:
			  state->keys[keyEvent->keysym.scancode] = KEY_RELEASE;
			  observers.notify(&event);
			  break;
		  case SDL_KEYDOWN:
			  state->keys[keyEvent->keysym.scancode] = KEY_PRESS;
			  observers.notify(&event);
			  break;
	  }
  }	
}

const Keyboard& KeyboardService::keyboard()
{
	sample();
	return *state;
}

}

