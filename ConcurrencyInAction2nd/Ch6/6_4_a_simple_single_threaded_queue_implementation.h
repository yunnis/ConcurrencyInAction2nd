#pragma once
#include "../head.h"

namespace _64
{
    template<typename T>
    class queue
    {
    private:
        struct node
        {
            T data;
            node* next;
            node(T data_)
            {
                data(std::move(data_));
                next = nullptr;
            }
        };
        std::unique_ptr<node> head; // 1
        node* tail; // 2
    public:
        queue():tail(nullptr){}
        queue(const queue& other) = delete;
        queue& operator=(const queue& other) = delete;
        std::shared_ptr<T> try_pop()
        {
            if (!head)
            {
                return std::shared_ptr<T>();
            }
            std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
            std::unique_ptr<node> const old_head = std::move(head);
            head = std::move(old_head->next); // 3
            if (!head)
            {
                tail = nullptr;
            }
            return res;
        }
        void push(T new_value)
        {
            std::unique_ptr<node> p(new node(std::move(new_value)));
            node* const new_tail = p.get();
            if (tail)
            {
                tail->next = std::move(p); // 4
            }
            else
            {
                head = std::move(p); // 5
            }
            tail = new_tail; // 6
        }
        // ********
        // 3,4 可能访问统一节点, 导致锁定同一互斥
        // 可以通过分离数据实现并发: 增加不含数据的虚拟节点 6_5
        // 等待数据弹出: 
        // ********
    };
}
