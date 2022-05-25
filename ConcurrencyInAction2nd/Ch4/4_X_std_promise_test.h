#pragma once
#include  "../head.h"

namespace _4X
{
    std::promise<int> p;
     std::shared_future<int> sf;
    void produce()
    {
        sf = p.get_future();
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        p.set_value(49);
    }
    void consume_1()
    {
        // promise 不能被多次get_future
        //std::future<int> f(p.get_future());
        //std::shared_future<int> f(p.get_future());
        std::shared_future<int> f(sf);
        std::cout << " thread: " << std::this_thread::get_id() << " wait promise. "<< f.get()  << "\n";
    }
    void consume_2()
    {
        //std::future<int> f(p.get_future());
        //std::shared_future<int> f(p.get_future());
        std::shared_future<int> f(sf);
        std::cout << " thread: " << std::this_thread::get_id() << " wait promise. "<< f.get()  << "\n";
    }
    void test()
    {
        //std::future<int> f(p.get_future());
        //std::cout << " promise test : future value from promise wait : "  << '\n';
        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::thread f1(produce);
        f1.join();
        //std::cout << " promise test : future value from promise is : " << f.get() << '\n';
        std::thread f2(consume_1);
        std::thread f3(consume_2);
        f2.join();
        f3.join();
    }
}
