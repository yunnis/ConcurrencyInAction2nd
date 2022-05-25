#pragma once

#include "head.h"

namespace printTen
{
    inline int i = 0;
    constexpr int num = 5;
    inline std::atomic_bool produced = true;
inline    size_t c1, c2;

    void mutexPrint()
    {
        c1 = 0; c2 = 0; i = 0;
        std::mutex m;
        std::thread t1([&m]()
            {
                int i1 = 0;
                while (i1 <= num)
                {
                    ++c1;
                    std::lock_guard<std::mutex>l(m);
                    if (i % 2 != 0)
                    {
                        ++i1;
                        ++i;
                        std::cout << i << " ";
                    }
                }
            });

        std::thread t2([&m]()
            {
                int i1 = 0;
                while (i1 <= num)
                {
                    ++c2;
                    std::lock_guard<std::mutex>l(m);
                    if (i % 2 == 0)
                    {
                        ++i1;
                        ++i;
                        std::cout << i << " ";
                    }
                }
            });

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        t1.join();
        t2.join();
        std::cout << "\n" << c1 << "   " << c2 << " \n";
    }
    void atomicFlagPrint()
    {
        c1 = 0; c2 = 0; i = 0;
        std::atomic_bool flag = false;
        std::atomic_int i = 0;
        int ii = 0;

        std::thread t1([&flag, &i, &ii]()
            {
                int c = 0;
                while (num > c)
                {
                    ++c1;
                    if (flag.load())
                    {
                        i.fetch_add(1);
                        ++ii;
                        ++c;
                        std::cout << " a    i: " << i << " ii: " << ii << " c: " << c << "\n";
                        flag.store(false);
                    }
                }
            });
        std::thread t2([&flag, &i, &ii]()
            {
                int c = 0;
                while (num > c)
                {
                    ++c2;
                    if (!flag.load())
                    {
                        i.fetch_add(1);
                        ++ii;
                        ++c;
                        std::cout << " b  i: " << i << " ii: " << ii << " c: " << c << "\n";
                        flag.store(true);
                    }
                }
            });
        t1.join();
        t2.join();
        std::cout << "\n" << c1 << "   " << c2 << " \n";
    }
    void conditionalVariablePrint()
    {
        c1 = 0; c2 = 0; i = 0;
        std::condition_variable cv;
        std::condition_variable cv_produce;
        std::mutex mut;

        size_t p1 = 0;
        std::thread produce_t([&p1, &mut, &cv, &cv_produce]()
            {
                while (true)
                {
                    if (produced)
                    {
                        ++p1;
                        std::unique_lock<std::mutex> lk(mut);
                        ++i;
                        cv.notify_all();
                        produced = false;
                        cv_produce.wait(lk, []()
                            {
                                return produced.load();
                            });
                        if (i >= num * 2)
                        {
                            produced = false;
                            cv.notify_all();
                            //std::cout << " produce_t end " << i << '\n';
                            break;
                        }
                    }
                }
            });
        std::thread t1([&mut, &cv, &cv_produce]()
            {
                while (true)
                {
                    ++c1;
                    std::unique_lock lk(mut);
                    cv.wait(lk, []()
                        {
                            return !produced;
                        });
                    std::cout << i << ' ';
                    produced.store(true);
                    cv_produce.notify_one();
                    if (i >= num * 2)
                    {
                        //std::cout << " t1 end " << i << '\n';
                        break;
                    }
                }
            });
        std::thread t2([&mut, &cv, &cv_produce]()
            {
                while (true)
                {
                    ++c2;
                    std::unique_lock lk(mut);
                    cv.wait(lk, []()
                        {
                            return !produced;
                        });
                    std::cout << i << ' ';
                    produced.store(true);
                    cv_produce.notify_one();
                    if (i >= num * 2)
                    {
                        //std::cout << " t2 end " << i << '\n';
                        break;
                    }

                }
            });
        produce_t.join();
        t1.join();
        t2.join();
        std::cout << "\n" << p1 << "  " << c1 << "   " << c2 << " \n";
    }
    // 仿照barrier 才用atomic wait notify_XX 搞一个无锁版本
    void atomicPrint2()
    {
        c1 = 0; c2 = 0; i = 0;
        std::atomic_int i = 0;
        std::thread t1([&i]
            {
                while (true)
                {
                    ++c1;
                    if (i > num * 2)
                    {
                        i.notify_one();
                        break;
                    }
                    if (i.load() % 2 == 0)
                    {
                        std::cout << i.load() << " ";
                        i.fetch_add(1);
                        i.notify_one();
                    }
                    else
                        i.wait(i);
                }
            });
        std::thread t2([&i]
            {
                while (true)
                {
                    ++c2;
                    if (i > num * 2)
                    {
                        i.notify_one();
                        break;
                    }
                    if (i.load() % 2 != 0)
                    {
                        std::cout << i.load() << " ";
                        i.fetch_add(1);
                        i.notify_one();
                    }
                    else
                        i.wait(i);
                }
            });
        t1.join();
        t2.join();
        //int ii = 0;
        //i.wait(ii);
        //i.notify_all();
        //i.notify_one();
        std::cout << "\n" << c1 << "   " << c2 << " \n";
    }
}
