#pragma once
#include "../head.h"

namespace _56
{
    std::atomic_int x(0), y(0), z(0);
    std::atomic_bool go(false);
    unsigned constexpr loop_count = 10;

    struct read_values
    {
        int x, y, z;
    };
    read_values v1[loop_count];
    read_values v2[loop_count];
    read_values v3[loop_count];
    read_values v4[loop_count];
    read_values v5[loop_count];
    void increment(std::atomic_int* var_to_inc, read_values* values)
    {
        while (!go)
        {
            std::this_thread::yield();
        }
        for (unsigned i = 0; i < loop_count; ++i)
        {
            values[i].x = x.load(std::memory_order_relaxed);
            values[i].y = y.load(std::memory_order_relaxed);
            values[i].z = z.load(std::memory_order_relaxed);
            var_to_inc->store(i + 1, std::memory_order_relaxed);
            std::this_thread::yield();
        }
    }
    void read_vals(read_values* values)
    {
        while (!go)
        {
            std::this_thread::yield();
        }
        for (unsigned i = 0; i < loop_count; ++i)
        {
            values[i].x = x.load(std::memory_order_relaxed);
            values[i].y = y.load(std::memory_order_relaxed);
            values[i].z = z.load(std::memory_order_relaxed);
            std::this_thread::yield();
        }
    }
    void print(read_values* v)
    {
        for (unsigned i = 0; i < loop_count; ++i)
        {
            if (i)
            {
                std::cout << ",";
            }
            std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
        }
        std::cout << std::endl;
    }
    void run()
    {
        std::thread t1(increment, &x, v1);
        std::thread t2(increment, &y, v2);
        std::thread t3(increment, &z, v3);
        std::thread t4(read_vals, v4);
        std::thread t5(read_vals, v5);
        go = true;
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        print(v1);
        print(v2);
        print(v3);
        print(v4);
        print(v5);
    }
}
