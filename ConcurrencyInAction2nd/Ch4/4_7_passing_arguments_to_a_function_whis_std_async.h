#pragma once
#include "../head.h"

namespace _47
{
    struct X
    {
        void foo(int, std::string const&);
        std::string bar(std::string const&);
    };
    X x;
    auto f1 = std::async(&X::foo, &x, 72, "hello");
    auto f2 = std::async(&X::bar, x, "goodbye");


    struct Y
    {
        double operator()(double);
    };
    Y y;
    // move-constructed from Y()
    auto f3 = std::async(Y(), 3.141);
    auto f4 = std::async(std::ref(y), 2.718);
    X baz(X&);
    auto f = std::async(baz, std::ref(x));
    class move_only
    {
    public:
        move_only();
        move_only(move_only&&);
        move_only(move_only const&) = delete;
        move_only& operator=(move_only&&);
        move_only& operator=(move_only const&) = delete;
        void operator()();
    };
    auto f5 = std::async(move_only());
    // ���߳���
    auto f6 = std::async(std::launch::async, Y(), 1.2);
    // ����wait() ��get() ��ִ��
    auto f7 = std::async(std::launch::deferred, baz, std::ref(x));
    // ʵ������ѡ�����з�ʽ
    auto f8 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x));
    auto f9 = std::async(baz, std::ref(x));

    std::future<int> f10 = std::async(std::launch::deferred, [] {return 1; });
    void testWait()
    {
        f10.wait();
        f7.wait();
    }

}
