#pragma once
#include "../head.h"

namespace _59
{
    std::atomic_int data[5];
    std::atomic_bool s1(false), s2(false);
    std::atomic_int s3(0);
    void thread_1()
    {
        data[0].store(42, std::memory_order_relaxed);
        data[1].store(42, std::memory_order_relaxed);
        data[2].store(42, std::memory_order_relaxed);
        data[3].store(42, std::memory_order_relaxed);
        data[4].store(42, std::memory_order_relaxed);
        s1.store(true, std::memory_order_release);
    }
    void thread_2()
    {
        while (!s1.load(std::memory_order_acquire))
        {
            s2.store(true, std::memory_order_release);
        }
    }
    void thread_3()
    {
        while (!s2.load(std::memory_order_acquire));
        assert(data[0].load(std::memory_order_relaxed) == 42);
        assert(data[1].load(std::memory_order_relaxed) == 42);
        assert(data[2].load(std::memory_order_relaxed) == 42);
        assert(data[3].load(std::memory_order_relaxed) == 42);
        assert(data[4].load(std::memory_order_relaxed) == 42);
    }
    void thread_4()
    {
        data[0].store(42, std::memory_order_relaxed);
        data[1].store(42, std::memory_order_relaxed);
        data[2].store(42, std::memory_order_relaxed);
        data[3].store(42, std::memory_order_relaxed);
        data[4].store(42, std::memory_order_relaxed);
        s3.store(1, std::memory_order_release);
    }
    void thread_5()
    {
        int expected = 1;
        while (!s3.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
        {
            expected = 1;
        }
    }
    void thread_6()
    {
        while (!s3.load(std::memory_order_acquire) < 2);
        assert(data[0].load(std::memory_order_relaxed) == 42);
        assert(data[1].load(std::memory_order_relaxed) == 42);
        assert(data[2].load(std::memory_order_relaxed) == 42);
        assert(data[3].load(std::memory_order_relaxed) == 42);
        assert(data[4].load(std::memory_order_relaxed) == 42);
    }

    void run(bool isOneFlag)
    {
        if (!isOneFlag)
        {
            std::thread t1(thread_1);
            std::thread t2(thread_2);
            std::thread t3(thread_3);
        }
        else
        {
            std::thread t4(thread_4);
            std::thread t5(thread_5);
            std::thread t6(thread_6);
        }
    }
}
