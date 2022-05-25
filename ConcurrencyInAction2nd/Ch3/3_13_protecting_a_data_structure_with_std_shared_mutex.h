#pragma once
#include "../head.h"

class dns_entry;
class dns_cache
{
	std::map < std::string, dns_entry > entries;
	mutable std::shared_mutex entry_mutex; // c++ 17
public:
	dns_entry find_entry(std::string const & domain) const
	{
		std::shared_lock<std::shared_mutex> slk(entry_mutex); // ������ == ����
		auto it = entries.find(domain);
		return it == entries.end() ? dns_entry() : it->second;
	}
	void update_or_add_entry(std::string const& domain, dns_entry const& dns_details)
	{
		// ���������߳�, ����find_entry
		std::lock_guard<std::shared_mutex> slk(entry_mutex); // ������ == д�� or use unique_lock
		entries[domain] = dns_details;
	}
};
