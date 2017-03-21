#pragma once
#include <vector>
#include "fast_delegate.hpp"

namespace base
{ 
	class Task_runner
	{
	public :
		static Task_runner& instance();

	public : 
		enum task_status { task_ok, task_end };

		//typedef unsigned (*task_fun)(float delta_ms);
		using Task_delegate = Fast_delegate<unsigned, float>;
		void add_task(Task_delegate f); 

	public :
		void run();

	private :
		Task_runner();
		std::vector<Task_delegate> tasks_; 

		unsigned last_tick_;
	};
}
