#pragma once
#include "../head.h"
#include <type_traits>

namespace _414
{
    template<typename F, typename A>
    // resulf_of delete in c++20
    //std::future<std::result_of<F(A&&)>::type> spawn_task(F&& f, A&& a)
    std::future<std::invoke_result_t<F(A&&)>> spawn_task(F&& f, A&& a)
    {
        typedef std::result_of<F(A&&)>::type result_type;
        std::packaged_task<result_type(A&&)> task(std::move(f));
        std::future<result_type> res(task.get_future());
        std::thread t(std::move(task), std::move(a));
        t.detach();
        return res;
    }
}
