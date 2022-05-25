#pragma once
#include  "../head.h"

// 对每个桶都加锁, 允许同个桶并发
// 仍有进步空间: 使用支持迭代器的线程安全的链表 进一步细化粒度
namespace _611
{
    template<typename Key, typename Value, typename Hash = std::hash<Key>>
    class threadsafe_lookup_table
    {
    private:
        class bucket_type
        {
        private:
            using bucket_value = std::pair<Key, Value>;
            using bucket_data = std::list<bucket_value>;
            using bucket_iterator = bucket_data::iterator;
            bucket_data data;
            mutable std::shared_mutex mutex; // 1

            bucket_iterator find_entry_for(Key const& key) const //2
            {
                return std::find_if(data.begin(), data.end(), [&](bucket_value const& item) {return item.first == key; });
            }
        public:
            Value value_for(Key const & key, Value const& default_value) const
            {
                std::shared_lock<std::shared_mutex> slk(mutex); // 3 读锁
                bucket_iterator const found_entry = find_entry_for(key);
                return found_entry == data.end() ? default_value : found_entry->second;
            }
            void add_or_update_mapping(Key const& key, Value const& value)
            {
                std::unique_lock<std::shared_mutex> ulk(mutex); // 4 写锁
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry == data.end())
                {
                    data.push_back(bucket_value(key, value));
                }
                else
                {
                    found_entry->second = value;
                }
            }
            void remove_mapping(Key const& key)
            {
                std::unique_lock<std::shared_mutex> ulk(mutex); // 5
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry != data.end())
                {
                    data.erase(found_entry);
                }
            }
        };

        std::vector<std::unique_ptr<bucket_type>> buckets; // 6
        Hash hasher;
        bucket_type& get_bucket(Key const& key) const // 7
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }
    public:
        using key_type = Key;
        using mapped_type = Value;
        using hash_type = Hash;
        threadsafe_lookup_table(unsigned num_buckets = 19, Hash const& hasher = Hash()) :
            buckets(num_buckets), hasher(hasher)
        {
            for (unsigned i =0;i<num_buckets;++i)
            {
                buckets[i].reset(new bucket_type);
            }
        }
        threadsafe_lookup_table(threadsafe_lookup_table const& other) = delete;
        threadsafe_lookup_table& operator=(const threadsafe_lookup_table &other) = delete;
        Value value_for(Key const& key, Value const& default_value=Value()) const
        {
            return get_bucket(key).value_for(key, default_value); // 8
        }
        void add_or_update_mapping(Key const& key, Value const& value)
        {
            get_bucket(key).add_or_update_mapping(key, value); // 9
        }
        void remove_mapping(Key const& key)
        {
            get_bucket(key).remove_mapping(key); // 10
        }
        // 6.12
        std::map<Key, Value> get_map() const
        {
            std::vector<std::unique_lock<std::shared_mutex>> locks;
            for(size_t i = 0; i < buckets.size(); ++i)
            {
                locks.push_back(std::unique_lock < std::shared_mutex >(buckets[i].mutex));
            }
            std::map<Key, Value> res;
            for(size_t i = 0; i < buckets.size(); ++i)
            {
                for(bucket_iterator it = buckets[i]->data.begin(); it != buckets[i]->data.end(); ++it)
                {
                    res.insert(*it);
                }
            }
        }
    };
}
