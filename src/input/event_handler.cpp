#include "event_handler.hpp"
#include <SDL.h>

namespace kvant {
namespace input {

	bool Event_handler::process()
	{
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) 
		{ 
			switch (e.type) 
			{ 
				case SDL_KEYUP:
					if (e.key.keysym.sym == SDLK_ESCAPE)
						return false;
					break;
				case SDL_QUIT:
					return false;
			}
		} 

		return true; 
	} 
}}
