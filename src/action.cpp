#include "./parsing/action.hpp"


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
