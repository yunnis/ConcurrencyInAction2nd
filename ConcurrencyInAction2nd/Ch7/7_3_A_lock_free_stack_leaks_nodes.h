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
            while (!head.compare_exchange_weak(new_node->next, new_node)); //  head�Ƿ����new_node->next, ��������new_node->next = head, ������head = new_node
        }
        std::shared_ptr<T> pop()
        {
            node* old_head = head.load();
            while (old_head && !head.compare_exchange_weak(old_head, old_head->next)); // 3 ��Ȼ����, ���ǲ�����ȴ�, ���ܽ������false
            return old_head ? old_head->next : std::shared_ptr<T>(); // 4 û������ڵ�, �ڴ�й©
        }
    };
}
