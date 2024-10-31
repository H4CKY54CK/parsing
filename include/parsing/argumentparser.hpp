#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "parsing/utils.hpp"
#include "parsing/action.hpp"
#include "parsing/actiongroup.hpp"


namespace parsing {
  // ArgumentParser declaration
  struct ArgumentParser {
    std::string name;
    std::string version;
    std::string usage;
    std::string description;
    bool explicit_name = false;
    std::deque<ActionGroup> groups = {
      ActionGroup(*this, "Positional Arguments"),
      ActionGroup(*this, "Options"),
    };
    bool help_added = false;
    bool help_removed = false;

    ArgumentParser(std::string name);

    auto add_argument_group(const std::string& name) -> ActionGroup&;
    auto add_argument(const std::string& value) -> Action&;
    auto add_argument(const std::initializer_list<std::string>& values) -> Action&;
    void add_help(bool value);
    void show_help() const;
    auto parse_args(const std::deque<std::string>& values) const -> std::unordered_map<std::string, Result>;
    auto parse_args(int argc, char** argv) const -> std::unordered_map<std::string, Result>;
  };
}
