#include "../head.h"

namespace _7_2
{
    template<typename T>
    class lock_free_stack
    {
    private:
        struct node
        {
            T data;
            node* next;
            node(T const& data_) :data(data_) {} // 1
        };
        std::atomic<node*> head;
    public:
        void push(T const& data)
        {
            node* const new_node = new node(data); // 2
            new_node->next = head.load(); // 3
            while (!head.compare_exchange_weak(new_node->next, new_node)); // 4 head�Ƿ����new_node->next, ��������new_node->next = head, ������head = new_node
        }
    };
}
