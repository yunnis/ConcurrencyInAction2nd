#pragma once
#include "../head.h"

namespace _27
{
	class joining_thread
	{
		std::thread t;
	public:
		joining_thread() noexcept = default;
		template<typename Callbale, typename ...Args>
		explicit joining_thread(Callbale&& func, Args&& ...args) noexcept :
			t(std::forward<Callbale>(func), std::forward<Args>(args)) {}
		explicit joining_thread(std::thread t_) noexcept:
			t(std::move(t_)) {}
		joining_thread(joining_thread&& other) noexcept:
			t(std::move(other.t)) {}
		joining_thread& operator=(joining_thread&& other) noexcept
		{
			if (joinable())
			{
				join();
			}
			t = std::move(other.t);
			return *this;
		}
		joining_thread& operator=(std::thread other) noexcept
		{
			if (joinable())
			{
				join();
			}
			t = std::move(other);
			return *this;
		}
		~joining_thread() noexcept
		{
			if (joinable())
			{
				join();
			}
		}
		void swap(joining_thread& other) noexcept
		{
			t.swap(other.t);
		}
		[[nodiscard]] std::thread::id get_id() const noexcept
		{
			return t.get_id();
		}
		[[nodiscard]] bool joinable() const noexcept
		{
			return t.joinable();
		}
		void join()
		{
			t.join();
		}
		void detach()
		{
			t.detach();
		}
		std::thread& as_thread() noexcept
		{
			return t;
		}
		const std::thread& as_thread() const noexcept
		{
			return t;
		}
	};
}
