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
#include <utility>
#include <vector>

#include "parsing/utils.hpp"

namespace parsing {
  // Action declaration
  struct Action {
    std::vector<std::string> flags_;
    std::string flags_string_;
    bool required_;
    std::string dest_;
    std::string metavar_;
    argtypes argtype_;
    std::string type_ {"string"};
    std::string default_ {""};
    std::string const_ {""};
    std::string nargs_ {"@"};
    actions action_ {actions::store};
    std::size_t min_nargs_ = 1;
    std::size_t max_nargs_ = 1;
    std::unordered_map<std::string, bool> user_provided;
    std::string help_ {""};

    explicit Action(const std::string& value);
    explicit Action(const std::initializer_list<std::string>& values);
    auto dest(const std::string& value) -> Action&;
    auto nargs(const std::string& value) -> Action&;
    auto nargs(std::size_t value) -> Action&;
    auto action(actions value) -> Action&;
    auto default_value(const std::string& value) -> Action&;
    auto const_value(const std::string& value) -> Action&;
    auto type(const std::string& value) -> Action&;
    auto metavar(const std::string& value) -> Action&;
    auto help(const std::string& value) -> Action&;
    auto required(bool value) -> Action&;
  private:
    void _check(const std::string& method);
  };

}
