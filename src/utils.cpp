#include "parsing/utils.hpp"


// DEFINITIONS

std::locale parsing::default_locale = std::locale("");

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

auto parsing::Result::as_bool() const -> bool {
  return bool(*this);
}

auto parsing::Result::as_int() const -> int {
  return int(*this);
}

auto parsing::Result::as_char() const -> char {
  return std::string(*this).c_str()[0];
}

auto parsing::Result::as_size_t() const -> std::size_t {
  return std::size_t(*this);
}

auto parsing::Result::as_string() const -> std::string {
  return std::string(*this);
}

auto parsing::Result::as_strings() const -> std::vector<std::string> {
  return std::vector<std::string>(*this);
}

auto parsing::Result::as_ints() const -> std::vector<int> {
  return std::vector<int>(*this);
}
