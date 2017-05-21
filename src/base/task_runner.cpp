#include "task_runner.hpp"

namespace kvant {
namespace base {

    Task_runner& Task_runner::instance()
    {
        static Task_runner inst;
        return inst;
    }

    void Task_runner::add_task(Task_delegate f)
    {
        tasks_.push_back(f);
    }

    void Task_runner::run()
    {
        // Iterate by index to avoid problem with iterator invalidation if new tasks are added.
        for (current_task_ = 0; current_task_ < tasks_.size(); ++current_task_)
        {
            tasks_[current_task_]();
        }
    }

    void Task_runner::end_current()
    {
        tasks_[current_task_] = tasks_.back();
        tasks_.pop_back();
        --current_task_;
    }

    Task_runner::Task_runner()
        : current_task_(0)
    {
        tasks_.reserve(64); // Speculative.
    }

} // namespace base
} // namespace kvant
