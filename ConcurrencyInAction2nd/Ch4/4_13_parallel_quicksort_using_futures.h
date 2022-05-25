#pragma once
#include "../head.h"

namespace _413
{
    // ����Ч��ʵ�ּ��ڰ���, c++17��׼��������ŵĲ������ذ汾
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
        // �����д�����, ��������������������߳�, ����Ч���½�
        std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));
        std::list<T> new_higher(parallel_quick_sort(std::move(input)));
        result.splice(result.end(), new_higher);
        result.splice(result.begin(), new_lower.get());
        return result;
    }
}
