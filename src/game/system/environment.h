#pragma once

#include <unordered_set>
#include <string>

class Environment {
  public:
    bool getFlag(std::string name);
    void setFlag(std::string name);

  private:
    std::unordered_set<std::string> flags_;
};
