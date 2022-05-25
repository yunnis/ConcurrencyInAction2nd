#pragma once
#include "../head.h"

namespace _63
{
    template<typename T>
    class threadsafe_queue
    {
    private:
        mutable  std::mutex mute;
        std::queue<std::shared_ptr<T>> data_queue;
        std::condition_variable data_cond;
    public:
        threadsafe_queue(){}
        void push(T new_value)
        {
            std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value))); // �ڴ����û�� 
            std::lock_guard<std::mutex> lock(mute);
            data_queue.push(data);
            data_cond.notify_one();
        }
        void wait_and_pop(T& value)
        {
            std::unique_lock<std::mutex> lock(mute);
            data_cond.wait(lock, [this]() {return !data_queue.empty(); });
            value = std::move(*data_queue.front());
            data_queue.pop();
        }
        std::shared_ptr<T>  wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(mute);
            data_cond.wait(lk, [this] { return !data_queue.empty(); });
            std::shared_ptr<T> res = data_queue.front();
            data_queue.pop();
            return res;
        }
        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lk(mute);
            if (data_queue.empty())
            {
                return false;
            }
            value = std::move(*data_queue.front());
            data_queue.pop();
            return true;
        }
        std::shared_ptr<T> try_pop()
        {
            std::lock_guard<std::mutex> lk(mute);
            if (data_queue.empty())
            {
                return std::shared_ptr<T>();
            }
            std::shared_ptr<T> res = data_queue.front();
            data_queue.pop();
            return res;
        }
        bool empty()
        {
            std::lock_guard<std::mutex> lk(mute);
            return data_queue.empty();
        }
    };
}
