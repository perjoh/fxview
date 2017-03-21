#include "task_runner.hpp"
#include <algorithm>
#include <SDL.h>

namespace base
{
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
		const unsigned now = ::SDL_GetTicks(); 
		const double delta_ms = std::abs(static_cast<double>(now - last_tick_)); 
		last_tick_ = now;

		size_t num_tasks = tasks_.size();
		for (size_t i = 0; i < num_tasks;)
		{
			const unsigned result = tasks_[i](static_cast<float>(delta_ms));

			if (task_end == result)
			{
				tasks_[i] = tasks_.back();
				tasks_.pop_back();
				num_tasks = tasks_.size();
			}
			else
			{
				++i;
			}
		}
	}

	Task_runner::Task_runner()
		: last_tick_(::SDL_GetTicks())
	{ 
	}
}
