#include "parsing.hpp"
#include <functional>



bool test_single_positional_argument() {
  std::vector<std::string> vec = {"one"};
  parsing::ArgumentParser parser = parsing::ArgumentParser("test_single_positional_argument");
  parser.add_argument("source");
  auto args = parser.parse_args(vec);

  if (args["source"].as_string() == vec.at(0)) {
    return true;
  }

  return false;
}



int main() {

  auto cool = [](bool i){ std::ostringstream oss; oss << std::boolalpha << "\x1b[" << (31 + i) << "m" << i << "\x1b[39m"; return oss.str(); };

  std::string ansi_bold = "\x1b[4m";
  std::string ansi_reset = "\x1b[0m";
  std::string bad = cool(false);
  std::string good = cool(true);

  std::cout << ansi_bold << "The answer is: " << bad << " (for reasons)" << ansi_reset << '\n';
  std::cout << "And then some other stuff...\n";
  std::cout << ansi_bold << "The answer is: " << good << " (for reasons)" << ansi_reset << '\n';

  // bool res;
  // res = test_single_positional_argument();
  // std::cout << "Test Positional Arguments (from string): " << cool(res) << '\n';
  // return 0;


  return 0;
}
