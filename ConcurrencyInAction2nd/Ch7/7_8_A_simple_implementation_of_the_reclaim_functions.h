#pragma once
#include "../head.h"

namespace _7_7
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
            if (threads_in_pop == 1) //
            {
                node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 当前线程把 待删链表 收归己有
                if (!--threads_in_pop) // pop()是唯一调用线程 不检测可能多线程调用pop() 某线程访问到已被删除的节点
                {
                    delete_nodes(nodes_to_delete); //
                }
                else if (nodes_to_delete) // 其他线程又调用了 pop()
                {
                    chain_pending_nodes(nodes_to_delete); // 把nodes_to_delete 放回 to_be_deleted
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
            while(node* const next = last->next) // 随着next前进到 待删链表 末端
            {
                last = next;
            }
            chain_pending_nodes(nodes, last);
        }
        void chain_pending_nodes(node* first, node* last)
        {
            last->next = to_be_deleted; //0
            while (!to_be_deleted.compare_exchange_weak(last->next, first)); //1 借循环保证 last->next指向正确
        }
        void chain_pending_node(node* n)
        {
            chain_pending_nodes(n, n); //
        }


        unsigned const max_hazard_pointers = 100;
        struct hazard_pointer
        {
            std::atomic<std::thread::id> id;
            std::atomic<void*> pointer;
        };
        hazard_pointer hazard_pointers[max_hazard_pointers];

        class hp_owner
        {
            hazard_pointer* hp;
        public:
            hp_owner(hp_owner const&) = delete;
            hp_owner operator=(hp_owner const&) = delete;
            hp_owner() :hp(nullptr)
            {
                for (unsigned i = 0; i < max_hazard_pointers; ++i)
                {
                    std::thread::id old_id;
                    if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) // 1
                    {
                        hp = &hazard_pointers[i];
                        break;
                    }
                }
                if (!hp) // 2
                {
                    throw std::runtime_error("No hazard pointers available");
                }
            }
            std::atomic<void*>& get_pointer()
            {
                return hp->pointer;
            }
            ~hp_owner() // 3
            {
                hp->pointer.store(nullptr);
                hp->id.store(std::thread::id());
            }
        };
        std::atomic<void*>& get_hazard_pointer_for_current_thread() // 4
        {
            thread_local  static hp_owner hazard; // 5
            return hazard.get_pointer(); // 6
        }

        bool outstanding_hazard_pointers_for(node* n)
        {
            for (unsigned i = 0; i < max_hazard_pointers; ++i)
            {
                if (hazard_pointers[i].pointer.load()==p)
                {
                    return true;
                }
                return false;
            }
        }
    public:
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
                do // 1 直到风险指针被置为head
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
                if (outstanding_hazard_pointers_for(old_head)) // 3 与隶属其他线程的风险指针逐一比较, 如果被其他线程指涉, 需要延迟回收
                {
                    reclaim_later(old_head); // 4
                }
                else
                {
                    delete old_head(); // 5
                }
                delete_nodes_with_no_hazards(); // 6 回收所有没被指涉的节点
            }
            return res;
        }
    };
}


template<typename T>
void do_delete(void* p)
{
    delete static_cast<T*>(p);
}

struct data_reclaim
{
    void* data;
    std::function<void(void*)> deleter;
    data_to_reclaim* next;
    data_to_reclaim(T* p) : data(p), deleter(&do_delete<T>), next(0) {}
};
