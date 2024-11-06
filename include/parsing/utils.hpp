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


namespace parsing {

  struct Action;
  struct ActionGroup;
  struct ArgumentParser;

  extern std::locale default_locale;

  extern std::unordered_map<std::size_t, std::string> level_colors;
  extern std::unordered_map<std::size_t, std::string> level_names;

  void log(std::size_t level, const std::string& name, const std::string& msg);
  void critical(const std::string& name, const std::string& msg);
  void error(const std::string& name, const std::string& msg);
  void warn(const std::string& name, const std::string& msg);
  void info(const std::string& name, const std::string& msg);
  void debug(const std::string& name, const std::string& msg);

  enum struct actions: std::uint8_t {store, store_true, store_false, store_const, append_const, append, extend, count, help, version};
  enum struct argtypes: std::uint8_t {positional, optional, boolean};

  extern std::unordered_map<actions, std::string> action_mapping;
  extern std::unordered_map<argtypes, std::string> argtype_mapping;

  auto repr(int value) -> std::string;
  auto repr(std::size_t value) -> std::string;
  auto repr(const std::string& value) -> std::string;
  auto join(const std::string& separator, const std::vector<std::string>& values) -> std::string;
  auto reprjoin(const std::string& separator, const std::vector<std::string>& values) -> std::string;
  auto reprjoin(const std::string& separator, const std::deque<std::string>& values) -> std::string;

  auto is_number(const std::string& value) -> bool;
  auto sorted_by_size(const std::vector<std::string>& values) -> std::vector<std::string>;
  auto to_upper(const std::string& value) -> std::string;
  auto get_dest(const std::vector<std::string>& values) -> std::string;
  auto get_argtype(const std::vector<std::string>& values) -> argtypes;
  auto get_required(const std::vector<std::string>& values) -> bool;

  // Result declaration
  struct Result {
    std::vector<std::string> values;

    void append(const std::string& value);
    void prepend(const std::string& value);
    auto size() -> std::size_t;
    auto empty() -> bool;
    void clear();

    explicit operator bool() const;
    explicit operator int() const;
    explicit operator std::string() const;
    explicit operator std::size_t() const;
    explicit operator std::vector<std::string>() const;
    explicit operator std::vector<int>() const;

    auto as_bool() const -> bool;
    auto as_int() const -> int;
    auto as_char() const -> char;
    auto as_size_t() const -> std::size_t;
    auto as_string() const -> std::string;
    auto as_strings() const -> std::vector<std::string>;
    auto as_ints() const -> std::vector<int>;
  };
}
