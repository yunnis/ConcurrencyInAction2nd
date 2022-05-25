#pragma once
#include "../head.h"

namespace _413
{
    // 更高效的实现见第八章, c++17标准库给出快排的并发重载版本
    template<typename T>
    std::list<T> parallel_quick_sort(std::list<T> input)
    {
        if (input.empty())
        {
            return input;
        }

        std::list<T> result;
        result.splice(result.begin(), input, input.begin());
        T const& pivot = *result.begin();
        auto divide_point = std::partition(input.begin(), input.end(), [&](T const& T) {return t < pivot; });
        std::list<T> lower_part;
        lower_part.splice(lower_part.end(), input, input.begin(), pivot);
        // 这里有大问题, 如果层数过多会产生大量线程, 导致效率下降
        std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));
        std::list<T> new_higher(parallel_quick_sort(std::move(input)));
        result.splice(result.end(), new_higher);
        result.splice(result.begin(), new_lower.get());
        return result;
    }
}
