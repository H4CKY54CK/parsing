#include "parsing.hpp"
#include "./testformatter.hpp"



void test_single_positional_value();


int main() {
  test_single_positional_value();
}


void test_single_positional_value() {
  std::deque<std::string> zero = {};
  std::deque<std::string> one = {"one"};
  std::deque<std::string> two = {"one", "two"};
  std::deque<std::string> three = {"one", "two", "three"};

  TestFormatter tf(24);
  std::unordered_map<std::string, parsing::Result> args;

  parsing::ArgumentParser parser = parsing::ArgumentParser::create_parser("one:default");
  parser.add_argument("source");
  args = parser.parse_args(one);
  if (args["source"].as_string() != one.front()) {
    tf.show_failure(parser.m.name, one);
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("one:explicit");
  parser.add_argument("sources").nargs(1);
  args = parser.parse_args(one);
  if (args["sources"].as_string() != one.front()) {
    tf.show_failure(parser.m.name, one);
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("two");
  parser.add_argument("sources").nargs(2);
  args = parser.parse_args(two);
  for (std::size_t ix = 0; ix < two.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != two[ix]) {
      tf.show_failure(parser.m.name, two);
    }
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("three");
  parser.add_argument("sources").nargs(3);
  args = parser.parse_args(three);
  for (std::size_t ix = 0; ix < three.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != three[ix]) {
      tf.show_failure(parser.m.name, three);
    }
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("zero-or-one");
  parser.add_argument("source").nargs("?");
  args = parser.parse_args(zero);
  if (args.count("source") != 0) {
    tf.show_failure(parser.m.name + "zero", zero);
  }
  args = parser.parse_args(one);
  if (args["source"].as_string() != one.front()) {
    tf.show_failure(parser.m.name + "one", one);
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("zero-or-more");
  parser.add_argument("sources").nargs("*");
  args = parser.parse_args(zero);
  if (args.count("sources") != 0) {
    tf.show_failure(parser.m.name + "zero", zero);
  }
  args = parser.parse_args(one);
  for (std::size_t ix = 0; ix < one.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != one[ix]) {
      tf.show_failure(parser.m.name + "one", one);
    }
  }
  args = parser.parse_args(two);
  for (std::size_t ix = 0; ix < two.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != two[ix]) {
      tf.show_failure(parser.m.name + "two", two);
    }
  }
  args = parser.parse_args(three);
  for (std::size_t ix = 0; ix < three.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != three[ix]) {
      tf.show_failure(parser.m.name + "three", three);
    }
  }
  tf.show_passed(parser.m.name);

  parser = parsing::ArgumentParser::create_parser("one-or-more");
  parser.add_argument("sources").nargs("+");
  args = parser.parse_args(one);
  for (std::size_t ix = 0; ix < one.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != one[ix]) {
      tf.show_failure(parser.m.name + "one", one);
    }
  }
  args = parser.parse_args(two);
  for (std::size_t ix = 0; ix < two.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != two[ix]) {
      tf.show_failure(parser.m.name + "two", one);
    }
  }
  args = parser.parse_args(three);
  for (std::size_t ix = 0; ix < three.size(); ++ix) {
    if (args["sources"].as_strings()[ix] != three[ix]) {
      tf.show_failure(parser.m.name + "three", one);
    }
  }
  tf.show_passed(parser.m.name);
}
