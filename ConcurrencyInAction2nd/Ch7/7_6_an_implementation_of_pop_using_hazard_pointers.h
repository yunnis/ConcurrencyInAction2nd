#pragma once
#include "../head.h"

namespace _7_6
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
            if (threads_in_pop == 1) //
            {
                node* nodes_to_delete = to_be_deleted.exchange(nullptr); // ��ǰ�̰߳� ��ɾ���� �չ鼺��
                if (!--threads_in_pop) // pop()��Ψһ�����߳� �������ܶ��̵߳���pop() ĳ�̷߳��ʵ��ѱ�ɾ���Ľڵ�
                {
                    delete_nodes(nodes_to_delete); //
                }
                else if (nodes_to_delete) // �����߳��ֵ����� pop()
                {
                    chain_pending_nodes(nodes_to_delete); // ��nodes_to_delete �Ż� to_be_deleted
                }
                delete old_head; //
            }
            else
            {
                chain_pending_node(old_head); //
                --threads_in_pop;
            }
        }
        void chain_pending_nodes(node* nodes)
        {
            node* last = nodes;
            while(node* const next = last->next) // ����nextǰ���� ��ɾ���� ĩ��
            {
                last = next;
            }
            chain_pending_nodes(nodes, last);
        }
        void chain_pending_nodes(node* first, node* last)
        {
            last->next = to_be_deleted; //0
            while (!to_be_deleted.compare_exchange_weak(last->next, first)); //1 ��ѭ����֤ last->nextָ����ȷ
        }
        void chain_pending_node(node* n)
        {
            chain_pending_nodes(n, n); //
        }
    public:
        std::atomic<void*> get_hazard_pointer_for_current_thread();
        bool outstanding_hazard_pointers_for(node* n);
        void reclaim_later(node* n);
        void delete_nodes_with_no_hazards();

        void push(T const& data)
        {
            node* const new_node = new node(data); // 
            new_node->next = head.load(); // 
            while (!head.compare_exchange_weak(new_node->next, new_node)); // 
        }
        std::shared_ptr<T> pop()
        {
            std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
            node* old_head = head.load();
            do // 
            {
                node* temp;
                do // 1 ֱ������ָ�뱻��Ϊhead
                {
                    hp.store(old_head);
                    old_head = head.load();
                } while (old_head != temp);
            } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
            hp.store(nullptr); // 2
            std::shared_ptr<T> res;
            if (old_head)
            {
                res.swap(old_head->data);
                if (outstanding_hazard_pointers_for(old_head)) // 3 �����������̵߳ķ���ָ����һ�Ƚ�, ����������߳�ָ��, ��Ҫ�ӳٻ���
                {
                    reclaim_later(old_head); // 4
                }
                else
                {
                    delete old_head(); // 5
                }
                delete_nodes_with_no_hazards(); // 6 ��������û��ָ��Ľڵ�
            }
            return res;
        }
    };
}


