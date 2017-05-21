#pragma once
#include <vector>
#include "fast_delegate.hpp"

namespace kvant {
namespace base {

    class Task_runner {
    public:
        static Task_runner& instance();

    public:
        using Task_delegate = Fast_delegate<void>;
        void add_task(Task_delegate f);

    public:
        void run();

        void end_current();

    private:
        Task_runner();
        std::vector<Task_delegate> tasks_;
        size_t current_task_;
    };

} // namespace base
} // namespace kvant
