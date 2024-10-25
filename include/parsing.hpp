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


namespace parsing {
  std::locale default_locale("");

  extern std::unordered_map<std::size_t, std::string> level_colors;
  extern std::unordered_map<std::size_t, std::string> level_names;

  inline void log(std::size_t level, const std::string& name, const std::string& msg);
  inline void critical(const std::string& name, const std::string& msg);
  inline void error(const std::string& name, const std::string& msg);
  inline void warn(const std::string& name, const std::string& msg);
  inline void info(const std::string& name, const std::string& msg);
  inline void debug(const std::string& name, const std::string& msg);

  // Forward declarations
  struct Action;
  struct ActionGroup;
  struct ArgumentParser;

  enum struct actions: std::uint8_t {store, store_true, store_false, store_const, append_const, append, extend, count, help, version};
  enum struct argtypes: std::uint8_t {positional, optional, boolean};

  extern std::unordered_map<actions, std::string> action_mapping;
  extern std::unordered_map<argtypes, std::string> argtype_mapping;

  inline auto repr(int value) -> std::string;
  inline auto repr(std::size_t value) -> std::string;
  inline auto repr(const std::string& value) -> std::string;
  inline auto join(const std::string& separator, const std::vector<std::string>& values) -> std::string;
  inline auto reprjoin(const std::string& separator, const std::vector<std::string>& values) -> std::string;
  inline auto reprjoin(const std::string& separator, const std::deque<std::string>& values) -> std::string;

  inline auto is_number(const std::string& value) -> bool;
  inline auto sorted_by_size(const std::vector<std::string>& values) -> std::vector<std::string>;
  inline auto to_upper(const std::string& value) -> std::string;
  inline auto get_dest(const std::vector<std::string>& values) -> std::string;
  inline auto get_argtype(const std::vector<std::string>& values) -> argtypes;
  inline auto get_required(const std::vector<std::string>& values) -> bool;


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

    auto as_bool() -> bool;
    auto as_int() -> int;
    auto as_char() -> char;
    auto as_size_t() -> std::size_t;
    auto as_string() -> std::string;
    auto as_strings() -> std::vector<std::string>;
    auto as_ints() -> std::vector<int>;
  };


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
    void show_help();
    auto parse_args(const std::vector<std::string>& vec) -> std::unordered_map<std::string, Result>;
    auto parse_args(int argc, char** argv) -> std::unordered_map<std::string, Result>;
  };
}


// DEFINITIONS

std::unordered_map<std::size_t, std::string> parsing::level_colors = {
  {50, "\x1b[41m"},
  {40, "\x1b[31m"},
  {30, "\x1b[33m"},
  {20, "\x1b[32m"},
  {10, "\x1b[36m"},
  {0,  "\x1b[00m"},
};

std::unordered_map<std::size_t, std::string> parsing::level_names = {
  {50, "critical"},
  {40, "error"},
  {30, "warning"},
  {20, "info"},
  {10, "debug"},
};

std::unordered_map<parsing::actions, std::string> parsing::action_mapping = {
  {actions::store, "store"},
  {actions::store_true, "store_true"},
  {actions::store_false, "store_false"},
  {actions::store_const, "store_const"},
  {actions::append_const, "append_const"},
  {actions::append, "append"},
  {actions::extend, "extend"},
  {actions::count, "count"},
  {actions::help, "help"},
  {actions::version, "version"},
};

std::unordered_map<parsing::argtypes, std::string> parsing::argtype_mapping = {
  {argtypes::positional, "positional"},
  {argtypes::optional, "optional"},
};

// Definitions
void parsing::log(std::size_t level, const std::string& name, const std::string& msg) {
  std::cerr << level_colors[level] << "[" << name << " " << level_names[level] << "]" << level_colors[0] << ": " << msg << '\n';
  std::cerr.flush();
}

void parsing::critical(const std::string& name, const std::string& msg) {
  log(50, name, msg);
}

void parsing::error(const std::string& name, const std::string& msg) {
  log(40, name, msg);
}

void parsing::warn(const std::string& name, const std::string& msg) {
  log(30, name, msg);
}

void parsing::info(const std::string& name, const std::string& msg) {
  log(20, name, msg);
}

void parsing::debug(const std::string& name, const std::string& msg) {
  log(10, name, msg);
}


auto parsing::repr(int value) -> std::string {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

auto parsing::repr(std::size_t value) -> std::string {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

auto parsing::repr(const std::string& value) -> std::string {
  if (value.find(' ') == value.npos) {
    return value;
  }
  if (value.find('\'') == value.npos) {
    return "'" + value + "'";
  }
  if (value.find('"') == value.npos) {
    return "\"" + value + "\"";
  }
  error("repr", "internal util function not sophisticated enough to handle strings containing all of: single quotes, double quotes, and spaces");
  std::quick_exit(1);
}

auto parsing::join(const std::string& separator, const std::vector<std::string>& values) -> std::string {
  std::string result;
  for (auto arg = values.begin(), end = values.end(); arg != end; ++arg) {
    result.append(*arg);
    if (arg + 1 != end) {
      result.append(separator);
    }
  }
  return result;
}

auto parsing::reprjoin(const std::string& separator, const std::vector<std::string>& values) -> std::string {
  std::string result;
  for (auto arg = values.begin(), end = values.end(); arg != end; ++arg) {
    result.append(repr(*arg));
    if (arg + 1 != end) {
      result.append(separator);
    }
  }
  return result;
}

auto parsing::reprjoin(const std::string& separator, const std::deque<std::string>& values) -> std::string {
  std::string result;
  for (auto arg = values.begin(), end = values.end(); arg != end; ++arg) {
    result.append(repr(*arg));
    if (arg + 1 != end) {
      result.append(separator);
    }
  }
  return result;
}

auto parsing::is_number(const std::string& value) -> bool {
  for (const auto& i : value) {
    if (!std::isdigit(i)) {
      return false;
    }
  }
  return (value.size() > 0);
}

auto parsing::sorted_by_size(const std::vector<std::string>& values) -> std::vector<std::string> {
  std::vector<std::string> vec(values.begin(), values.end());
  std::sort(vec.begin(), vec.end(), [](const std::string& left, const std::string& right){ return left.size() < right.size(); });
  return vec;
}

auto parsing::to_upper(const std::string& value) -> std::string {
  std::string other = value;
  for (auto& c: other) {
    c = std::toupper(c, default_locale);
  }
  return other;
}

auto parsing::get_dest(const std::vector<std::string>& values) -> std::string {
  std::string result;
  for (const auto& arg: values) {
    if (result.size() < arg.size()) {
      result = arg;
    }
  }
  for (int i = 0; i < 2; ++i) {
    if (result.substr(0, 1) == "-") {
      result = result.substr(1);
      continue;
    }
    break;
  }
  return result;
}

auto parsing::get_argtype(const std::vector<std::string>& values) -> parsing::argtypes {
  for (const auto& flag : values) {
    if (flag.substr(0, 1) != "-") {
      if (values.size() > 1) {
        error("Action", "cannot use multiple flags unless they all have leading hyphens");
        std::quick_exit(1);
      }
      return argtypes::positional;
    }
  }
  return argtypes::optional;
}

auto parsing::get_required(const std::vector<std::string>& values) -> bool {
  return (get_argtype(values) == argtypes::positional) ? true : false;
}


// Action definition
parsing::Action::Action(const std::string& value) : Action({value}) {}

parsing::Action::Action(const std::initializer_list<std::string>& values)
  : flags_(values)
  , flags_string_(join("/", sorted_by_size(values)))
  , required_(get_required(values))
  , dest_(get_dest(values))
  , metavar_(to_upper(get_dest(values)))
  , argtype_(get_argtype(values))
{
  if (values.size() == 0) {
    error("Action", "must have at least one option string");
    std::quick_exit(1);
  }
}

// Convenience via method chaining
auto parsing::Action::dest(const std::string& value) -> parsing::Action& {
  _check("dest");
  if (value.empty()) {
    error("Action", "argument cannot contain an empty dest");
    std::quick_exit(1);
  }
  dest_ = value;
  return *this;
}

auto parsing::Action::nargs(const std::string& value) -> parsing::Action& {
  _check("nargs");
  if (value == "?") {
    min_nargs_ = 0;
    max_nargs_ = 1;
  }
  else if (value == "*") {
    min_nargs_ = 0;
    max_nargs_ = 0;
  }
  else if (value == "+") {
    min_nargs_ = 1;
    max_nargs_ = 0;
  }
  else {
    error("Action", "nargs must be either a positive integer or one of: '?', '*', '+'");
    std::quick_exit(1);
  }
  nargs_ = value;
  return *this;
}

auto parsing::Action::nargs(std::size_t value) -> parsing::Action& {
  _check("nargs");
  min_nargs_ = value;
  max_nargs_ = value;
  action_ = actions::extend;
  return *this;
}

auto parsing::Action::action(actions value) -> parsing::Action& {
  _check("action");
  // if (nargs_ != "@") {
  //   error("Action", "argument action cannot be set after explicitly setting nargs");
  //   std::quick_exit(1);
  // }
  switch (value) {
    case actions::store_true: {
      nargs(0);
      const_value("true");
      break;
    }
    case actions::store_false: {
      nargs(0);
      const_value("false");
      break;
    }
    case actions::help:
    case actions::version:
    case actions::append_const:
    case actions::store_const: {
      nargs(0);
      break;
    }
    case actions::count: {
      nargs(0);
      const_value("1");
      break;
    }
    default: {
      break;
    }
  }
  action_ = value;
  return *this;
}

auto parsing::Action::default_value(const std::string& value) -> parsing::Action& {
  _check("default_value");
  default_ = value;
  return *this;
}

auto parsing::Action::const_value(const std::string& value) -> parsing::Action& {
  _check("const_value");
  const_ = value;
  return *this;
}

auto parsing::Action::type(const std::string& value) -> parsing::Action& {
  _check("type");
  type_ = value;
  return *this;
}

auto parsing::Action::metavar(const std::string& value) -> parsing::Action& {
  _check("metavar");
  metavar_ = value;
  return *this;
}

auto parsing::Action::help(const std::string& value) -> parsing::Action& {
  _check("help");
  help_ = value;
  return *this;
}

auto parsing::Action::required(bool value) -> parsing::Action& {
  _check("required");
  required_ = value;
  return *this;
}

void parsing::Action::_check(const std::string& method) {
  if (user_provided.count(method) == 1) {
    error("Action", "cannot provide ." + method + " twice");
    std::quick_exit(1);
  }
  user_provided[method] = true;
}



// ActionGroup definition
parsing::ActionGroup::ActionGroup(ArgumentParser& parent, std::string name) : parent(parent), name(std::move(name)) {}

auto parsing::ActionGroup::add_argument(const std::string& value) -> parsing::Action& {
  if (value.substr(0, 1) == "-") {
    return add_argument({value});
  }
  arguments.emplace_back(value);
  return arguments.back();
}

auto parsing::ActionGroup::add_argument(const std::initializer_list<std::string>& values) -> parsing::Action& {
  for (auto& group : parent.groups) {
    for (auto& flag : values) {
      if (group.flags.count(flag) > 0) {
        error("Action", "duplicate flags: " + group.flags.at(flag).flags_string_ + " uses " + flag);
        std::quick_exit(1);
      }
    }
  }
  arguments.emplace_back(values);
  if (get_argtype(values) == argtypes::optional) {
    for (auto& flag : values) {
      flags.emplace(flag, arguments.back());
    }
  }
  return arguments.back();
}


// Result definition
void parsing::Result::append(const std::string& value) {
  values.emplace_back(value);
}

void parsing::Result::prepend(const std::string& value) {
  values.emplace(values.begin(), value);
}

auto parsing::Result::size() -> std::size_t {
  return values.size();
}

auto parsing::Result::empty() -> bool {
  return values.size() == 0;
}

void parsing::Result::clear() {
  values.clear();
}

parsing::Result::operator bool() const {
  if (values.size() == 0) {
    return false;
  }
  if (values.at(0) == "true") {
    return true;
  }
  else if (values.at(0) == "false") {
    return false;
  }
  throw std::invalid_argument("not a boolean");
}

parsing::Result::operator int() const {
  if (values.size() == 0) {
    return 0;
  }
  if (!is_number(values.at(0))) {
    throw std::invalid_argument("not an integer");
  }
  return std::stoi(values.at(0));
}

parsing::Result::operator std::string() const {
  if (values.size() == 0) {
    return "";
  }
  return values.at(0);
}

parsing::Result::operator std::size_t() const {
  if (values.size() == 0) {
    return 0;
  }
  if (!is_number(values.at(0))) {
    throw std::invalid_argument("not an integer");
  }
  return std::stoul(values.at(0));
}

parsing::Result::operator std::vector<std::string>() const {
  if (values.size() == 0) {
    return {};
  }
  return values;
}

parsing::Result::operator std::vector<int>() const {
  if (values.size() == 0) {
    return {};
  }
  std::vector<int> vec;
  for (const auto& item : values) {
    if (!is_number(item)) {
      throw std::invalid_argument("not all items were integers");
    }
    vec.emplace_back(std::stoul(item));
  }
  return vec;
}

auto parsing::Result::as_bool() -> bool {
  return bool(*this);
}

auto parsing::Result::as_int() -> int {
  return int(*this);
}

auto parsing::Result::as_char() -> char {
  return std::string(*this).c_str()[0];
}

auto parsing::Result::as_size_t() -> std::size_t {
  return std::size_t(*this);
}

auto parsing::Result::as_string() -> std::string {
  return std::string(*this);
}

auto parsing::Result::as_strings() -> std::vector<std::string> {
  return std::vector<std::string>(*this);
}

auto parsing::Result::as_ints() -> std::vector<int> {
  return std::vector<int>(*this);
}


// ArgumentParser definition
parsing::ArgumentParser::ArgumentParser(std::string name) : name(std::move(name)) {
  add_help(true);
}

auto parsing::ArgumentParser::add_argument_group(const std::string& name) -> parsing::ActionGroup& {
  groups.emplace_back(*this, name);
  return groups.back();
}

auto parsing::ArgumentParser::add_argument(const std::string& value) -> parsing::Action& {
  argtypes at_ = value.substr(0, 1) == "-" ? argtypes::optional : argtypes::positional;
  switch (at_) {
    case argtypes::positional: {
      return groups.at(0).add_argument(value);
    }
    case argtypes::optional: {
      return groups.at(1).add_argument(value);
    }
    default: {
      error("ArgumentParser", "unrecognized argument type: " + argtype_mapping[at_]);
      std::quick_exit(1);
    }
  }
}

auto parsing::ArgumentParser::add_argument(const std::initializer_list<std::string>& values) -> parsing::Action& {
  argtypes at_ = get_argtype(values);
  switch (at_) {
    case argtypes::positional: {
      return groups.at(0).add_argument(values);
    }
    case argtypes::optional: {
      return groups.at(1).add_argument(values);
    }
    default: {
      error("ArgumentParser", "unrecognized argument type: " + argtype_mapping[at_]);
      std::quick_exit(1);
    }
  }
}

void parsing::ArgumentParser::add_help(bool value) {
  if (value) {
    if (not help_added) {
      groups.at(1).add_argument({"--help", "-h"}).action(actions::help).help("Show this menu and exit.");
      help_added = true;
    }
  }
  else {
    if (not help_removed) {
      for (auto& flag : groups.at(1).arguments.front().flags_) {
        if (flag != "-h" and flag != "--help") {
          return;
        }
        groups.at(1).flags.erase(flag);
      }
      groups.at(1).arguments.erase(groups.at(1).arguments.begin());
      help_removed = true;
    }
  }
}

void parsing::ArgumentParser::show_help() {
  std::ostringstream oss;
  // Usage
  if (not usage.empty()) {
    oss << usage;
  }
  else {
    oss << "Usage: " << name;
    for (auto& group : groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        if (argument.nargs_ == "@") {
          oss << " " << argument.metavar_;
        }
        else if (argument.nargs_ == "?") {
          oss << " [" << argument.metavar_ << " ...]";
        }
        else if (argument.nargs_ == "*") {
          oss << " [" << argument.metavar_ << " ...]";
        }
        else if (argument.nargs_ == "+") {
          oss << " " << argument.metavar_ << " [" << argument.metavar_ << " ...]";
        }
      }
    }
    for (auto& group : groups) {
      if (group.arguments.empty()) {
        continue;
      }
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::positional) {
          continue;
        }
        if (argument.min_nargs_ == argument.max_nargs_ and argument.max_nargs_ == 0) {
          oss << " [" << argument.flags_string_ << "]";
        }
        else if (argument.min_nargs_ == argument.max_nargs_ and argument.max_nargs_ == 1) {
          oss << " [" << argument.flags_string_ << " " << argument.metavar_ << "]";
        }
        else if (argument.min_nargs_ == 0) {
          oss << " [" << argument.flags_string_ << " " << "[" << argument.metavar_ << " ...]]";
        }
        else if (argument.min_nargs_ == 1) {
          oss << " [" << argument.flags_string_ << " " << argument.metavar_ << "]";
        }
      }
    }
    oss << '\n';
  }
  oss << '\n';

  auto spaces = [](std::size_t n){ return std::string(n, ' '); };

  // Argument Groups
  for (auto& group : groups) {
    if (group.arguments.empty()) {
      continue;
    }
    oss << group.name << '\n';
    for (auto& argument : group.arguments) {
      if (argument.argtype_ == argtypes::positional) {
        // Positionals
        oss << spaces(4) << argument.dest_ << '\n';
        if (not argument.help_.empty()) {
          oss << spaces(8) << argument.help_ << '\n';
        }
      }
      else if (argument.argtype_ == argtypes::optional) {
        // Optionals
        oss << spaces(4) << argument.flags_string_;
        if (not (argument.action_ == actions::store_true
              or argument.action_ == actions::store_false
              or argument.action_ == actions::store_const
              or argument.action_ == actions::append_const
              or argument.action_ == actions::help
              or argument.action_ == actions::version
              or argument.action_ == actions::count
        )) {
          oss << "=" << argument.metavar_;
        }
        oss << '\n';
        if (not argument.help_.empty()) {
          oss << spaces(8) << argument.help_ << '\n';
        }
      }
      else {
        error("Help", "invalid argtype; could not complete help menu formatting");
        std::quick_exit(1);
      }
    }
    oss << '\n';
  }

  // Description
  if (not description.empty()) {
    oss << description << '\n';
  }

  // Flush or we won't see text when piped, as I found out
  std::cout << oss.str();
  std::cout.flush();
}

auto parsing::ArgumentParser::parse_args(int argc, char** argv) -> std::unordered_map<std::string, parsing::Result> {
  std::vector<std::string> vec(argv, argv + argc);
  return parse_args(vec);
}

auto parsing::ArgumentParser::parse_args(const std::vector<std::string>& vec) -> std::unordered_map<std::string, parsing::Result> {
  std::unordered_map<std::string, Result> results;
  std::deque<std::string> remaining;

  for (auto arg = vec.begin(), end = vec.end(); arg != end; ++arg) {

    // If it looks like an optional argument
    if (arg->substr(0, 1) == "-") {

      // Handle --
      if ((*arg) == "--") {
        for (++arg; arg != end; ++arg) {
          remaining.emplace_back(*arg);
        }
        break;
      }

      auto group = groups.begin();
      auto gend = groups.end();

      // Get group where flag might be in
      for (; group != gend; ++group) {
        if (group->flags.count((*arg)) == 1) {
          break;
        }
      }

      // Unrecognized optional argument
      if (group == gend) {
        error("parser", "unrecognized optional argument: " + (*arg));
        std::quick_exit(1);
      }

      // Handle valid optional arguments
      auto opt = group->flags.at(*arg);
      if (results.count(opt.dest_) == 1) {
        error("parser", "optional argument already provided: " + opt.flags_string_);
        std::quick_exit(1);
      }

      switch (opt.action_) {
        // Handle non-consuming options
        case actions::store_true:
        case actions::store_false:
        case actions::store_const: {
          results[opt.dest_].prepend(opt.const_);
          break;
        }
        case actions::count:
        case actions::append_const: {
          results[opt.dest_].append(opt.const_);
          break;
        }
        case actions::version: {
          std::cout << version << '\n';
          std::cout.flush();
          std::quick_exit(1);
        }
        case actions::help: {
          show_help();
          std::quick_exit(1);
        }

        // Handle consuming options
        case actions::store:
        case actions::extend: {
          if (opt.action_ == actions::store) {
            results[opt.dest_].clear();
          }
          while ((arg + 1) != end) {
            arg++;
            if (arg->substr(0, 1) == "-") {
              error("parser", opt.flags_string_ + " expects exactly " + repr(opt.min_nargs_) + " value(s), but got ambiguous value: " + repr(*arg));
              std::quick_exit(1);
            }
            results[opt.dest_].append(*arg);
            if (opt.max_nargs_ > 0 and results[opt.dest_].size() == opt.max_nargs_) {
              break;
            }
          }
          if (results[opt.dest_].size() < opt.min_nargs_) {
            if (opt.min_nargs_ == opt.max_nargs_) {
              error("parser", opt.flags_string_ + " expects exactly " + repr(opt.min_nargs_) + " value(s), but got " + repr(results[opt.dest_].size()));
              std::quick_exit(1);
            }
            error("parser", opt.flags_string_ + " expects at least " + repr(opt.min_nargs_) + " value(s), but got " + repr(results[opt.dest_].size()));
            std::quick_exit(1);
          }
          break;
        }
        case actions::append: {
          error("parser", "not yet implemented: " + action_mapping[opt.action_]);
          std::quick_exit(1);
        }
        default: {
          error("parser", "unrecognized action: " + action_mapping[opt.action_]);
          std::quick_exit(1);
        }
      }
      continue;
    }

    // Positional
    remaining.emplace_back(*arg);
  }

  // Add default arguments
  for (auto& group : groups) {
    for (auto& argument : group.arguments) {
      if (not argument.default_.empty()) {
        if (results[argument.dest_].empty()) {
          results[argument.dest_].append(argument.default_);
        }
      }
    }
  }

  // Check for required optionals
  for (auto& group : groups) {
    for (auto& argument : group.arguments) {
      if (argument.argtype_ == argtypes::positional) {
        continue;
      }
      if (argument.required_ and results[argument.dest_].empty()) {
        error("parser", "missing required optional argument: " + argument.flags_string_);
        std::quick_exit(1);
      }
    }
  }

  // Now for the confusing task of arranging positional arguments when positional argument count can be variable
  std::size_t lower = 0;
  bool exact = true;

  // Get the min and max number of required arguments, and whether there are any that don't need to be exact
  for (auto& group : groups) {
    for (const auto& argument: group.arguments) {
      if (argument.argtype_ == argtypes::optional) {
        continue;
      }
      if (argument.nargs_ == "?" or argument.nargs_ == "*" or argument.nargs_ == "+") {
        exact = false;
      }
      lower += argument.min_nargs_;
    }
  }

  // Check for too few arguments
  if (remaining.size() < lower) {
    std::size_t subtotal = 0;
    for (auto& group : groups) {
      for (auto& argument : group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        subtotal += argument.min_nargs_;
        if (subtotal > remaining.size()) {
          error("parser", "missing positional argument: " + argument.flags_string_);
          std::quick_exit(1);
        }
      }
    }
  }

  // If it's exact, then we have exactly the right amount
  if (exact) {
    for (auto& group : groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        for (std::size_t ix = 0; ix < argument.min_nargs_; ++ix) {
          results[argument.dest_].append(remaining.front());
          remaining.pop_front();
        }
      }
    }
  }

  // And finally, the check for variable number of arguments
  else {
    // Create layout of minimum required arguments per index
    std::size_t known = 0;
    for (auto& group : groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        if (argument.nargs_ == "@") {
          known += argument.min_nargs_;
        }
      }
    }

    // Distribute them
    for (auto& group : groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        std::size_t pix = remaining.size();

        // Exactly X
        if (argument.nargs_ == "@") {
          for (std::size_t ix = 0; ix < argument.min_nargs_; ++ix) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          }
        }

        // 0 or 1
        else if (argument.nargs_ == "?") {
          if ((remaining.size() - known) > 0) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          }
        }

        // 0 or more and 1 or more
        else if (argument.nargs_ == "*" or argument.nargs_ == "+") {
          for (std::size_t ix = 0; ix < (pix - known); ++ix) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          }
        }
      }
    }
  }

  // If any left over, then we need to error
  if (not remaining.empty()) {
    error("ArgumentParser", "unrecognized arguments: " + reprjoin(" ", remaining));
    std::quick_exit(1);
  }

  // Finally, return
  return results;
}
