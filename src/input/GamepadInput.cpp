#include "GamepadInput.hpp"
#include "Logging.hpp"
#include <SDL.h>
#include <cassert>

namespace util {

	GamepadInput::GamepadInput()
		: gameController_(Initialize())
	{ }

	GamepadInput::~GamepadInput()
	{ 
		if (gameController_) 
        {
			::SDL_GameControllerClose(gameController_);
		}
	}

	bool GamepadInput::Initialized() const 
    {
		return gameController_ != nullptr;
	}


	std::pair<float, float> GamepadInput::ReadLeftAnalog() 
    { 
		assert(Initialized());

		return std::make_pair(
			ReadNormalizedAnalog(leftStick[0], SDL_CONTROLLER_AXIS_LEFTX),
			ReadNormalizedAnalog(leftStick[1], SDL_CONTROLLER_AXIS_LEFTY));
	}

	std::pair<float, float> GamepadInput::ReadRightAnalog() 
    { 
		assert(Initialized());

		return std::make_pair(
			ReadNormalizedAnalog(rightStick[0], SDL_CONTROLLER_AXIS_RIGHTX),
			ReadNormalizedAnalog(rightStick[1], SDL_CONTROLLER_AXIS_RIGHTY));
	}

	float GamepadInput::ReadLeftTrigger() 
    { 
		return ReadNormalizedAnalog(leftTrigger, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
	}

	float GamepadInput::ReadRightTrigger() 
    {
		return ReadNormalizedAnalog(rightTrigger, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}


	SDL_GameController* GamepadInput::Initialize() 
    { 
		const int numJoysticks = ::SDL_NumJoysticks(); 
		for (int i = 0; i < numJoysticks; ++i) 
        {
			if (::SDL_IsGameController(i)) 
            {
				return ::SDL_GameControllerOpen(i);
			}
		}

		return nullptr;
	}

}