#include "parsing/argumentparser.hpp"



// ArgumentParser definition
parsing::ArgumentParser::ArgumentParser(M m) : m(std::move(m)) {}

parsing::ArgumentParser::ArgumentParser(const ArgumentParser& other) : m(other.m) {}

parsing::ArgumentParser& parsing::ArgumentParser::operator=(const ArgumentParser& other) {
  if (this == &other) {
    return *this;
  }
  ArgumentParser temp(other);
  std::swap(m, temp.m);
  return *this;
}

parsing::ArgumentParser::ArgumentParser(ArgumentParser&& other) : m(std::exchange(other.m, M{})) {}

parsing::ArgumentParser& parsing::ArgumentParser::operator=(ArgumentParser&& other) {
  ArgumentParser temp(std::move(other));
  std::swap(m, temp.m);
  return *this;
}

parsing::ArgumentParser parsing::ArgumentParser::create_parser(std::string value) {
  ArgumentParser ap(M{std::move(value)});
  ap.add_argument_group("Positional Arguments");
  ap.add_argument_group("Options");
  ap.add_help(true);
  return ap;
}

auto parsing::ArgumentParser::add_argument_group(const std::string& name) -> ActionGroup& {
  m.groups.emplace_back(*this, name);
  return m.groups.back();
}

auto parsing::ArgumentParser::add_argument(const std::string& value) -> Action& {
  argtypes at_ = value.substr(0, 1) == "-" ? argtypes::optional : argtypes::positional;
  switch (at_) {
    case argtypes::positional: {
      return m.groups.at(0).add_argument(value);
    }
    case argtypes::optional: {
      return m.groups.at(1).add_argument(value);
    }
    default: {
      error("ArgumentParser", "unrecognized argument type: " + argtype_mapping[at_]);
      std::quick_exit(1);
    }
  }
}

auto parsing::ArgumentParser::add_argument(const std::initializer_list<std::string>& values) -> Action& {
  argtypes at_ = get_argtype(values);
  switch (at_) {
    case argtypes::positional: {
      return m.groups.at(0).add_argument(values);
    }
    case argtypes::optional: {
      return m.groups.at(1).add_argument(values);
    }
    default: {
      error("ArgumentParser", "unrecognized argument type: " + argtype_mapping[at_]);
      std::quick_exit(1);
    }
  }
}

void parsing::ArgumentParser::add_help(bool value) {
  if (value) {
    if (not m.help_added and not m.help_removed) {
      m.groups.at(1).add_argument({"--help", "-h"}).action(actions::help).help("Show this menu and exit.");
      m.help_added = true;
    }
  }
  else {
    if (not m.help_removed and m.help_added) {
      for (auto& flag : m.groups.at(1).arguments.front().flags_) {
        if (flag != "-h" and flag != "--help") {
          return;
        }
        m.groups.at(1).flags.erase(flag);
      }
      m.groups.at(1).arguments.erase(m.groups.at(1).arguments.begin());
      m.help_removed = true;
    }
  }
}

void parsing::ArgumentParser::show_help() const {
  std::ostringstream oss;
  // Usage
  if (not m.usage.empty()) {
    oss << m.usage;
  }
  else {
    oss << "Usage: " << m.name;
    for (auto& group : m.groups) {
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
    for (auto& group : m.groups) {
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
  for (auto& group : m.groups) {
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
  if (not m.description.empty()) {
    oss << m.description << '\n';
  }

  // Flush or we won't see text when piped, as I found out
  std::cout << oss.str();
  std::cout.flush();
}


auto parsing::ArgumentParser::parse_args(const std::deque<std::string>& values) const -> std::unordered_map<std::string, Result> {
  std::deque<std::string> args(values.begin(), values.end());
  std::unordered_map<std::string, Result> results;
  std::deque<std::string> remaining;

  for (auto arg = args.begin(), end = args.end(); arg != end; ++arg) {
    // If it looks like an optional argument
    if (arg->substr(0, 1) == "-") {

      // Handle --
      if (*arg == "--") {
        for (++arg; arg != end; ++arg) {
          remaining.emplace_back(*arg);
        }
        break;
      }

      auto group = m.groups.begin();

      // Get group where flag might be in
      for (; group != m.groups.end(); ++group) {
        if (group->flags.count(*arg) == 1) {
          break;
        }

        if (arg->find("=") != arg->npos) {
          auto left = arg->substr(0, arg->find("="));
          if (group->flags.count(left) == 1) {
            args.insert(arg + 1, arg->substr(left.size() + 1));
            *arg = left;
            end = args.end();
            break;
          }
        }
      }

      // Unrecognized optional argument
      if (group == m.groups.end()) {
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
          std::cout << m.version << '\n';
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
            ++arg;
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
  for (auto& group : m.groups) {
    for (auto& argument : group.arguments) {
      if (not argument.default_.empty()) {
        if (results[argument.dest_].empty()) {
          results[argument.dest_].append(argument.default_);
        }
      }
    }
  }

  // Check for required optionals
  for (auto& group : m.groups) {
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
  for (auto& group : m.groups) {
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
    for (auto& group : m.groups) {
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
    for (auto& group : m.groups) {
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
    for (auto& group : m.groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }
        known += argument.min_nargs_;
      }
    }

    // Distribute them
    for (auto& group : m.groups) {
      for (auto& argument: group.arguments) {
        if (argument.argtype_ == argtypes::optional) {
          continue;
        }

        if (argument.nargs_ == "@") {
          for (std::size_t ix = 0; ix < argument.min_nargs_; ++ix) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
            known--;
          }
        }

        else if (argument.nargs_ == "?") {
          if (remaining.size() > known) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          }
        }

        else if (argument.nargs_ == "*") {
          while (remaining.size() > known) {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          }
        }

        else if (argument.nargs_ == "+") {
          known--;
          do {
            results[argument.dest_].append(remaining.front());
            remaining.pop_front();
          } while (remaining.size() > known);
        }

      }
    }
  }

  // If any left over, then we need to error
  if (not remaining.empty()) {
    error("ArgumentParser", "(this is probably a bug in the parser, honestly) unrecognized arguments: " + reprjoin(" ", remaining));
    std::quick_exit(1);
  }

  // Finally, return
  return results;
}


auto parsing::ArgumentParser::parse_args(int argc, char** argv) const -> std::unordered_map<std::string, Result> {
  return parse_args(std::deque<std::string>(argv, argv+argc));
}

