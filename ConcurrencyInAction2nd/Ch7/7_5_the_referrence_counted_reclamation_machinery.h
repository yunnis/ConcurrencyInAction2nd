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
            // ** ��һֱ���ڸ߸���, to_be_delete��һֱ���� �޷��ͷ�, Ҳ��һ���ڴ�й©
            // ���������ͨ�� ����ָ�� ʵ�ֻ���
            if (threads_in_pop == 1) // 1
            {
                node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 2 ��ǰ�̰߳� ��ɾ���� �չ鼺��
                if (!--threads_in_pop) // 3 pop()��Ψһ�����߳� �������ܶ��̵߳���pop() ĳ�̷߳��ʵ��ѱ�ɾ���Ľڵ�
                {
                    delete_nodes(nodes_to_delete); // 4
                }
                else if (nodes_to_delete) // 5 �����߳��ֵ����� pop()
                {
                    chain_pending_nodes(nodes_to_delete); // 6 ��nodes_to_delete �Ż� to_be_deleted
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
            while(node* const next = last->next) // 9 ����nextǰ���� ��ɾ���� ĩ��
            {
                last = next;
            }
            chain_pending_nodes(nodes, last);
        }
        void chain_pending_nodes(node* first, node* last)
        {
            last->next = to_be_deleted; // 10
            while (!to_be_deleted.compare_exchange_weak(last->next, first)); // 11 ��ѭ����֤ last->nextָ����ȷ
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
                res.swap(old_head->next); // 3 ֻҪ���ܾͻ�����ɾ���ڵ�
            }
            try_reclaim(old_head); // 4 �ӽڵ���ȡ����, �����Ǹ���ָ��
            return res;
        }
    };
}

