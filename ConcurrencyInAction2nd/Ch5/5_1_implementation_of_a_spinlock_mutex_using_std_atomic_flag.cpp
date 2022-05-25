#include "../head.h"

namespace _51
{
    // 自旋锁, 忙等本质是循环, 占用大量cpu时间
    class spinlock_mutex
    {
        std::atomic_flag flag;
    public:
        spinlock_mutex() {}
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                ;
            }
        }
        void unlock()
        {
            flag.clear(std::memory_order_release);
        }
    };
}
