#pragma once
#include  "../head.h"

namespace _612
{
    template<typename T>
    class threadsafe_list
    {
        struct node // 1
        {
            std::mutex m;
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
            node() :next() {} // 2
            node(T const& value) :data(std::make_shared<T>(value)) {} // 3
        };
        node head;
    public:
        threadsafe_list() {}
        ~threadsafe_list()
        {
            remove_if([](node const&) {return true; });
        }
        threadsafe_list(threadsafe_list const& other) = delete;
        threadsafe_list& operator=(threadsafe_list const& other) = delete;
        void push_front(T const& value) // 头插法
        {
            std::unique_ptr<node> new_node(new node(value)); // 4
            std::lock_guard<std::mutex> lk(head.m);
            new_node->next = std::move(head.next); // 5 锁住头, 正确读取其next, 并指向新节点next
            head.next = std::move(new_node); // 6 再将头的next指向新节点
        }
        template<typename Function>
        void for_each(Function f) // 7
        {
            node* current = &head;
            std::unique_lock<std::mutex> lk(head.m); // 8
            while(node* const next = current->next.get()) // 9
            {
                std::unique_lock<std::mutex> next_lk(next->m); // 10 锁定下个节点, 就可以释放上个节点的锁
                lk.unlock(); // 11
                f(*next->data); // 12
                current = next;
                lk = std::move(next_lk); // 13
            }
        }
        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p) // 14
        {
            node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next.m);
                lk.unlock();
                if(p(*next->data)) // 15
                {
                    return next->data; // 16
                }
                current = next;
                lk = std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }
        template<typename Predicate>
        void remove_if(Predicate p) // 17
        {
            node* current = &head;
            std::unique_lock<std::mutex>lk(head.m);
            while(node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                if (p(*next->data)) // 18
                {
                    std::unique_ptr<node> old_next = std::move(current->next);
                    current->next = std::move(next->next); // 19
                    next_lk.unlock();
                } // 20
                else
                {
                    lk.unlock(); //21
                    current = next;
                    lk = std::move(next_lk);
                }
            }
        }
    };
}
