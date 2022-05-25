#pragma once
#include "../head.h"

namespace _7_5
{
    template<typename T>
    class lock_free_stack
    {
    private:
        struct node
        {
            std::shared_ptr<T> data; // 
            node* next;
            node(T const& data_) :data(data_) {} // 
        };
        std::atomic<unsigned> threads_in_pop;
        std::atomic<node*> head;
        std::atomic<node*> to_be_deleted;
        static void delete_nodes(node* nodes)
        {
            while(nodes)
            {
                node* next = nodes->next;
                delete nodes;
                nodes = next;
            }
        }
        void try_reclaim(node* old_head)
        {
            // ** 若一直处于高负载, to_be_delete将一直增加 无法释放, 也是一种内存泄漏
            // 上述情况可通过 风险指针 实现回收
            if (threads_in_pop == 1) // 1
            {
                node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 2 当前线程把 待删链表 收归己有
                if (!--threads_in_pop) // 3 pop()是唯一调用线程 不检测可能多线程调用pop() 某线程访问到已被删除的节点
                {
                    delete_nodes(nodes_to_delete); // 4
                }
                else if (nodes_to_delete) // 5 其他线程又调用了 pop()
                {
                    chain_pending_nodes(nodes_to_delete); // 6 把nodes_to_delete 放回 to_be_deleted
                }
                delete old_head; // 7
            }
            else
            {
                chain_pending_node(old_head); // 8
                --threads_in_pop;
            }
        }
        void chain_pending_nodes(node* nodes)
        {
            node* last = nodes;
            while(node* const next = last->next) // 9 随着next前进到 待删链表 末端
            {
                last = next;
            }
            chain_pending_nodes(nodes, last);
        }
        void chain_pending_nodes(node* first, node* last)
        {
            last->next = to_be_deleted; // 10
            while (!to_be_deleted.compare_exchange_weak(last->next, first)); // 11 借循环保证 last->next指向正确
        }
        void chain_pending_node(node* n)
        {
            chain_pending_nodes(n, n); // 12
        }
    public:
        void push(T const& data)
        {
            node* const new_node = new node(data); // 
            new_node->next = head.load(); // 
            while (!head.compare_exchange_weak(new_node->next, new_node)); // 
        }
        std::shared_ptr<T> pop()
        {
            ++threads_in_pop;
            node* old_head = head.load();
            while (old_head && !head.compare_exchange_weak(old_head, old_head->next)); // 
            std::shared_ptr<T> res;
            if (old_head)
            {
                res.swap(old_head->next); // 3 只要可能就回收已删除节点
            }
            try_reclaim(old_head); // 4 从节点提取数据, 而不是复制指针
            return res;
        }
    };
}

