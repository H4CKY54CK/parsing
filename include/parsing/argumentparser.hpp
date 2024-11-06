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
#include "parsing/action.hpp"
#include "parsing/actiongroup.hpp"


namespace parsing {
  // ArgumentParser declaration
  struct ArgumentParser {
    struct M {
      std::string name;
      std::string version = "";
      std::string usage = "";
      std::string description = "";
      std::deque<ActionGroup> groups = {};
      bool explicit_name = false;
      bool help_added = false;
      bool help_removed = false;
    } m;

    explicit ArgumentParser(M m);

    ArgumentParser(const ArgumentParser& other);
    ArgumentParser& operator=(const ArgumentParser& other);
    ArgumentParser(ArgumentParser&& other);
    ArgumentParser& operator=(ArgumentParser&& other);

    static ArgumentParser create_parser(std::string value);

    auto add_argument_group(const std::string& name) -> ActionGroup&;
    auto add_argument(const std::string& value) -> Action&;
    auto add_argument(const std::initializer_list<std::string>& values) -> Action&;
    void add_help(bool value);
    void show_help() const;
    auto parse_args(const std::deque<std::string>& values) const -> std::unordered_map<std::string, Result>;
    auto parse_args(int argc, char** argv) const -> std::unordered_map<std::string, Result>;
  };
}
