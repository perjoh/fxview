#include "task_runner.hpp"
#include <algorithm>

namespace base
{
	task_runner& task_runner::instance()
	{
		static task_runner inst;
		return inst;
	}

	void task_runner::add_task(task_delegate f)
	{
		tasks_.push_back(f);
	}

	void task_runner::run(float delta_ms)
	{
		for (	auto task_it = tasks_.begin();
				task_it !=  tasks_.end();)
		{
			const unsigned result = (*task_it)(delta_ms);

			if (task_end == result)
			{
				task_it = tasks_.erase(task_it); 
			}
			else
			{
				++task_it;
			}
		}
	}
}
