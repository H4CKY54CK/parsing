#include <iostream>
#include <string>



struct TestFormatter {
  std::size_t max_width = 0;

  TestFormatter(std::size_t max_width);

  void show_passed(const std::string& name);
  void show_failure(const std::string& name, const std::deque<std::string>& expected);
  void show_specific_failure(const std::string& name, const std::string& extra, const std::deque<std::string>& expected);
};



TestFormatter::TestFormatter(std::size_t max_width): max_width(max_width) {}

void TestFormatter::show_passed(const std::string& name) {
  std::cerr << "tests:" << name << "\x1b[" << max_width << "G" << " -> " <<  "\x1b[32m[PASSED]\x1b[39m" << '\n';
  std::cerr.flush();
}

void TestFormatter::show_failure(const std::string& name, const std::deque<std::string>& expected) {
  std::cerr << "tests:" << name << "\x1b[" << max_width << "G" << " -> " <<  "\x1b[31m[FAILED]\x1b[39m" << " | {";
  for (std::size_t ix = 0; ix < expected.size(); ++ix) {
    std::cerr << std::quoted(expected[ix]);
    if (ix < (expected.size() - 1)) {
      std::cerr << ", ";
    }
  }
  std::cerr << "}" << '\n';
  std::cerr.flush();
}
