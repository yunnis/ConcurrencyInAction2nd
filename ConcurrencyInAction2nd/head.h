#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <chrono>
#include <memory>
#include <set>
#include <map>
#include <condition_variable>
#include <queue>
#include <future>
#include <iostream>
#include <type_traits>
#include <format>
// c++20 将flex_barrier 与barrier 合起来了, 并且正式加入标准库
#include <barrier>
#include <latch>
#include <assert.h>
#include <experimental/generator>

struct dns_entry
{
	unsigned long long ip;
};

struct result_chunk
{
    void set_chunk(unsigned i, unsigned num_threads, const result_chunk& process);
};

struct data_chunk
{
};

struct data_block
{
};

struct data_source
{
    bool done();
    data_block get_next_data_block();
};
struct data_sink
{
    void write_data(const result_chunk& move);
};
