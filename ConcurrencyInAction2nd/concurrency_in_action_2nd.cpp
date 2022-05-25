// ConcurrencyInAction2nd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Ch2/2_3_yuing_RAII_to_wait_for_a_thread_to_complte.h"
#include "PrintTen.h"
#include "Ch3/3_8_hierarchical_mutex.h"
#include "Ch4/4_5_tread_safe_queue_using_condition_variables.h"
#include "Ch4/4_X_std_promise_test.h"
#include "CH5/5_4_sequential_consistency_implies_a_total_ordering.h"
#include "Ch5/5_6_relaxed_operations_on_multiple_threads.h"

std::once_flag init_flag;
static int si;
void init_si(int i)
{
    si = i;
}
void init()
{
    std::call_once(init_flag, init_si, 003);
}

int main()
{
    {
        init();
    }

    {
        _56::run();
    }

    {
        _54::run();
        std::cout << " \n end 54 \n";
    }

    {
        threadsafe_queue<int> q;
        q.empty();
    }

    std::promise<int> p;
    p.set_value(1);
    std::future<int> f(p.get_future());
    //std::future<int> f1(p.get_future());
    std::cout << " f : "<< f.get()<<'\n';

    _4X::test();

    // test hierarchical mutex
    {
        hierarchical_mutex low(100);
        hierarchical_mutex mid(200);
        hierarchical_mutex high(300);

        //auto a = [&]()
        //{
        //    std::lock_guard<hierarchical_mutex> l(low);
        //};
        //auto b = [&]()
        //{
        //    std::lock_guard<hierarchical_mutex> l(low);
        //};
    	auto a = [&]()
        {
            low.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            low.unlock();
        };
        auto b = [&]()
        {
            low.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            low.unlock();
        };

        std::thread t1(a);
        std::thread t2(b);
        t1.join();
        t2.join();
    }

    std::cout << "\n using mutex\n";
    printTen::mutexPrint();
    std::cout << "\n using condition variable\n";
    printTen::conditionalVariablePrint();
    std::cout << "\n using atomic flag\n";
    printTen::atomicFlagPrint();
    std::cout << "\n using atomic like condition variable but need c++20\n";
    printTen::atomicPrint2();
    std::cout << "Hello World!\n";
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
