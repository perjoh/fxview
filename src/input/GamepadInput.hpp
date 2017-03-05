#pragma once
#include <utility>
#include <array>
#include <SDL_gamecontroller.h>

namespace util {

	class GamepadInput 
    {
	public :
		GamepadInput();
		~GamepadInput();

	public :
		bool Initialized() const;

	public :
		std::pair<float, float> ReadLeftAnalog();
		std::pair<float, float> ReadRightAnalog();
		float ReadLeftTrigger();
		float ReadRightTrigger();
		//const std::array<float, 4>& ReadButtons();

	private :
		::SDL_GameController* Initialize();

	private :
		class AnalogInput 
        {
		public :
			AnalogInput()
				: maxInput(65536/4)
				, deadZone(0.1f)
			{ }

			float GetNormalized(int value) 
            { 
				if (!(value < 0) && maxInput < value)
					maxInput = value;
				else if (value < 0 && maxInput < -value)
					maxInput = -value;

				const float result = static_cast<float>(value) / static_cast<float>(maxInput);
				const float absResult = std::abs(result);

				if (absResult < deadZone)
					return 0.0f;

				const float factor = (absResult - deadZone)/(absResult*(1.0f - deadZone));
				return factor*result;
			}

		private :
			int maxInput;
			float deadZone;
		};

		AnalogInput leftStick[2];
		AnalogInput rightStick[2];
		AnalogInput leftTrigger;
		AnalogInput rightTrigger; 

		::SDL_GameController* gameController_;

		inline float ReadNormalizedAnalog(	AnalogInput& analogInput, 
											::SDL_GameControllerAxis axis) 
		{ 
			return analogInput.GetNormalized(
				::SDL_GameControllerGetAxis(
					gameController_, 
					axis)); 
		}
	};

}