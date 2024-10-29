#include "./parsing/actiongroup.hpp"

#include "./parsing/argumentparser.hpp"


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
