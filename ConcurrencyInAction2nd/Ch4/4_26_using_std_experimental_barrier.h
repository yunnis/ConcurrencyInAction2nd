#pragma once
#include  "../Ch2/2_7_a_joining_thread_class.h"
#include "../head.h"

namespace _426
{
    result_chunk process(data_chunk);
    std::vector<data_chunk> divide_into_chunks(data_block data, unsigned num_threads);

    void split_source();

    void process_data(data_source& source, data_sink &sink)
    {
        unsigned const concurrency = std::thread::hardware_concurrency();
        unsigned const num_threads = (concurrency > 0) ? concurrency : 2;

        std::barrier sync(num_threads);
        std::vector<_27::joining_thread> threads(num_threads);

        std::vector<data_chunk> chunks;
        result_chunk result;

        for (unsigned i = 0; i < num_threads; ++i)
        {
            threads[i] = _27::joining_thread([&, i]
                {
                    while (!source.done())//6
                    {
                        if (!i) // 1 1==0时切割数据块
                        {
                            data_block current_block = source.get_next_data_block();
                            chunks = divide_into_chunks(current_block, num_threads);
                        }
                        sync.arrive_and_wait();//2 所有线程等待0号线程切割完毕
                        result.set_chunk(i, num_threads, process(chunks[i]));//3
                        sync.arrive_and_wait();//4 等待其他线程都处理玩才能让0号线程进行 sink的写入操作
                        if (!i)//5
                        {
                            sink.write_data(std::move(result));
                        }
                    }
                });
        }
        std::barrier(num_threads, [&]
            {
                sink.write_data(std::move(result));
                split_source();
                return -1;
            });
    }
}
