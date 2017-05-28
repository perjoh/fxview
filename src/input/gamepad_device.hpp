#pragma once
#include <utility>

namespace kvant {
namespace input {

    class Input_device {
    public:
        virtual ~Input_device() = default;

    public:
        virtual void update_device() = 0;

    public:
        enum Analog_stick { left,
                            right };
        using Stick_position = std::pair<float, float>;
        virtual Stick_position read_stick(Analog_stick) = 0;

        enum Analog_trigger { left_a,
                              left_b,
                              right_a,
                              right_b };
        virtual float read_trigger(Analog_trigger) = 0;

        enum Analog_button { button_x,
                             button_y,
                             button_a,
                             button_b };
        virtual float read_button(Analog_button) = 0;
    };

    const Input_device& get_gamepad_device_any();

} // namespace input
} // namespace kvant