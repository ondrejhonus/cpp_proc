#pragma once

#include <string>

#include "manager.hpp"

class sorting {
 public:
  sorting();
  ~sorting() = default;

  std::vector<ProcessManager::Proc> get_sorted_procs(
      std::vector<ProcessManager::Proc>& processes, std::string method,
      bool is_asc);
};
