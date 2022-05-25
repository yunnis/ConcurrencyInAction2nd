#include "../head.h"

namespace _7_3
{
    template<typename T>
    class lock_free_stack
    {
    private:
        struct node
        {
            std::shared_ptr<T> data; // 1
            node* next;
            node(T const& data_) :data(data_) {} // 2
        };
        std::atomic<node*> head;
    public:
        void push(T const& data)
        {
            node* const new_node = new node(data); // 
            new_node->next = head.load(); // 
            while (!head.compare_exchange_weak(new_node->next, new_node)); //  head是否等于new_node->next, 不等于则new_node->next = head, 等于则head = new_node
        }
        std::shared_ptr<T> pop()
        {
            node* old_head = head.load();
            while (old_head && !head.compare_exchange_weak(old_head, old_head->next)); // 3 虽然无锁, 但是不代表等待, 可能结果总是false
            return old_head ? old_head->next : std::shared_ptr<T>(); // 4 没有清理节点, 内存泄漏
        }
    };
}
