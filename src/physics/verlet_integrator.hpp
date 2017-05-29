#pragma once

namespace kvant {
namespace physics {

    // p_new = 2*p_cur - p_old + acceleration()*delta_t*delta_t;
    
    template <typename T>
    T accelerate(const T& p)
    {
    }

    template <typename T>
    T integrate(const T& p, const T& p_old, float dt)
    {
        return 2.0f*p - p_old + accelerate(p)*dt*dt;
    }
}
}
