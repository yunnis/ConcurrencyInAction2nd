#pragma once
#include "../head.h"

template<typename IT, typename T>
struct accumulate_block
{
	void operator()(IT first, IT last, T& result)
	{
		result = std::accumulate(first, last, result);
	}
};

template<typename IT, typename T>
T parallet_accumulate(IT first, IT last, T init)
{
	const unsigned long long length = std::distance(first, last);
	if (!length)
	{
		return init;
	}
	const unsigned long long min_per_thread = 25;
	const unsigned long long max_therads = (length + min_per_thread - 1) / min_per_thread;
	const unsigned long long hardware_threads = std::thread::hardware_concurrency();
	const unsigned long long num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_therads);
	const unsigned long long block_size = length / num_threads;
	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads - 1);
	IT block_start = first;
	for(unsigned long long i=0;i<(num_threads-1);++i)
	{
		IT block_end = block_start;
		std::advance(block_end, block_size);
		threads[i] = std::thread(accumulate_block<IT, T>(), block_start, block_end, std::ref(results[i]));
		block_start = block_end;
	}
	accumulate_block < IT, T >()(block_start, last, results[num_threads - 1]);
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	return std::accumulate(results.begin(), results.end(), init);
}
