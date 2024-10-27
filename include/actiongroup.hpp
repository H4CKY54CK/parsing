#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hpp"
#include "action.hpp"
// #include "argumentparser.hpp"


namespace parsing {
  // ActionGroup declaration
  struct ActionGroup {
    ArgumentParser& parent;
    std::string name;
    std::deque<Action> arguments;
    std::unordered_map<std::string, Action&> flags;

    ActionGroup(ArgumentParser& parent, std::string name);

    auto add_argument(const std::string& value) -> Action&;
    auto add_argument(const std::initializer_list<std::string>& values) -> Action&;
  };



}
