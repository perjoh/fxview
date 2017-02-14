#pragma once
#include <vector>
#include "fast_delegate.hpp"

namespace base
{ 
	class task_runner
	{
	public :
		static task_runner& instance();

	public : 
		enum task_status { task_ok, task_end };

		//typedef unsigned (*task_fun)(float delta_ms);
		typedef fast_delegate<unsigned, float> task_delegate;
		void add_task(task_delegate f); 

	public :
		void run(float delta_ms);

	private :
		task_runner() = default;
		std::vector<task_delegate> tasks_; 
	};
}
