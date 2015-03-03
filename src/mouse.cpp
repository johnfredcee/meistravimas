#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include "observer.h"
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"
#include "service.h"
#include "servicecheckout.h"
#include "mouse.h"

namespace venk
{
// service

bool MouseService::initialise(MouseService* self) {
  if (self != nullptr) {
	self->state = new Mouse();
	self->state->x  = 0;
	self->state->y  = 0;
	self->state->dx = 0;
	self->state->dy = 0;
	self->state->firstsample = true;
	self->state->sampleTime = SDL_GetTicks() / 1000.0f;
	self->state->wheel = 0;
  }
  return true;
}

bool MouseService::shutdown(MouseService* self) {
  if ( self != nullptr ) {
	delete self->state;
  }
  return true;
}


void MouseService::sample()
{
  SDL_Event event;
  SDL_MouseMotionEvent *mouseMotionEvent;
  SDL_MouseButtonEvent *mouseButtonEvent;
  SDL_MouseWheelEvent  *mouseWheelEvent;
  double sampleTime;
  state->dx = 0.0f;
  state->dy = 0.0f;
  while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEWHEEL)) {
	  switch (event.type)
	  {
		  case SDL_MOUSEMOTION:
			  mouseMotionEvent = (SDL_MouseMotionEvent*)(&event);
			  if (!state->firstsample) {
				  state->dx = (float)( mouseMotionEvent->xrel ) * (sampleTime - mouseMotionEvent->timestamp) * 0.001f;
				  state->dy = (float)( mouseMotionEvent->yrel ) * (sampleTime - mouseMotionEvent->timestamp) * 0.001f;		  
			  } else {
				  state->firstsample = false;
				  sampleTime = mouseMotionEvent->timestamp;
			  }
			  state->sampleTime = sampleTime;
			  state->x = mouseMotionEvent->x;
			  state->y = mouseMotionEvent->y;
			  observers.notify(&event);
			  break;
		  case SDL_MOUSEBUTTONUP:
			  mouseButtonEvent = (SDL_MouseButtonEvent*)(&event);
			  state->buttons[mouseButtonEvent->button] = SDL_RELEASED;
			  observers.notify(&event);
			  break;
		  case SDL_MOUSEBUTTONDOWN:
			  mouseButtonEvent = (SDL_MouseButtonEvent*)(&event);
			  state->buttons[mouseButtonEvent->button] = SDL_PRESSED;
			  observers.notify(&event);
			  break;				
		  case SDL_MOUSEWHEEL:
			  mouseWheelEvent = (SDL_MouseWheelEvent*)(&event);
			  state->wheel += mouseWheelEvent->y;
			  observers.notify(&event);
			  break;
	  }
  }	

}

const Mouse& MouseService::mouse() {
	sample();
	return *state;
}


}
