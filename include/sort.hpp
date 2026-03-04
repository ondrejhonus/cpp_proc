#pragma once

#include "cpm.hpp"
#include <string>

class sorting
{
public:
	sorting();
	~sorting() = default;

	std::vector<cpm::Proc> get_sorted_procs(std::vector<cpm::Proc>& processes, std::string method, bool is_asc);
private:
};
