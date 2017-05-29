#include "gamepad_device.hpp"
#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <cassert>

namespace kvant {
namespace input {

    class Gamepad_device : public Input_device {
    public:
        Gamepad_device();
        ~Gamepad_device();

    public:
        void update_device() override;

        Stick_position read_stick(Analog_stick) override;
        float read_trigger(Analog_trigger) override;
        float read_button(Analog_button) override;

    private:
        class Analog_input {
        public:
            float get_normalized(int value)
            {
				if (!(value < 0) && max_input_ < value)
				{
					max_input_ = value;
				}
				else if (value < 0 && max_input_ < -value)
				{
					max_input_ = -value;
				}

                const float result = static_cast<float>(value) / static_cast<float>(max_input_);
                const float absResult = std::abs(result);

                if (absResult < dead_zone_)
                    return 0.0f;

                const float factor = (absResult - dead_zone_) / (absResult * (1.0f - dead_zone_));
                return factor * result;
            }

        private:
            int max_input_{65536 / 4};
            float dead_zone_{0.1f};
        };

        Analog_input left_stick_[2];
        Analog_input right_stick_[2];
        Analog_input left_triggers_[2];
        Analog_input right_triggers_[2];

        ::SDL_GameController* game_controller_{nullptr};

        inline float read_normalized_analog(Analog_input& analog_input, ::SDL_GameControllerAxis axis)
        {
            return analog_input.get_normalized(::SDL_GameControllerGetAxis(game_controller_, axis));
        }
    };

    SDL_GameController* find_controller()
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

    Gamepad_device::Gamepad_device()
        : game_controller_(find_controller())
    {
    }

    Gamepad_device::~Gamepad_device()
    {
        if (game_controller_)
        {
            ::SDL_GameControllerClose(game_controller_);
        }
    }

    void Gamepad_device::update_device()
    {
    }

    const Input_device& get_gamepad_device_any()
    {
        static Gamepad_device gamepad;
        return gamepad;
    }

    Input_device::Stick_position Gamepad_device::read_stick(Input_device::Analog_stick stick)
    {
        if (stick == right)
        {
            return std::make_pair( read_normalized_analog(right_stick_[0], SDL_CONTROLLER_AXIS_RIGHTX),
                read_normalized_analog(right_stick_[1], SDL_CONTROLLER_AXIS_RIGHTY));
        }

        return std::make_pair(read_normalized_analog(left_stick_[0], SDL_CONTROLLER_AXIS_LEFTX),
                              read_normalized_analog(left_stick_[1], SDL_CONTROLLER_AXIS_LEFTY));
    }

    float Gamepad_device::read_trigger(Analog_trigger trigger)
    {
        const ::SDL_GameControllerAxis sdl_trigger = (trigger == left) ? SDL_CONTROLLER_AXIS_TRIGGERLEFT
                                                                       : SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

        return read_normalized_analog(left_triggers_[0], sdl_trigger);
    }

    float Gamepad_device::read_button(Analog_button)
    {
        return 0.0f;
    }

} // namespace input
} // namespace kvant