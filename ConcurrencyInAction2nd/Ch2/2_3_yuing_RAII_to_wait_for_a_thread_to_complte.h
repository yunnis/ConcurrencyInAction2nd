#pragma once
#include "2_1_a_function_that_returns_while_a_thread_still_has_access_to_local_variables.h"

namespace _23
{
	class thread_guard
	{
		std::thread t_;
	public:
		explicit thread_guard(std::thread t) : t_(std::move(t)) {}
		~thread_guard()
		{
			if (t_.joinable())
			{
				t_.join();
			}
		}
		thread_guard(thread_guard const&) = delete;
		thread_guard& operator=(thread_guard const&) = delete;
	};
	void f()
	{
		int state = 0;
		_21::func my_func1(state);
		std::thread t(my_func1);
		thread_guard g(std::move(t));
	}
}
