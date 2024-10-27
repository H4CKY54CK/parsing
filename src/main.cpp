#include "parsing.hpp"

#include <iostream>
#include <string>
#include <vector>




void show_args(std::unordered_map<std::string, parsing::Result> args);
auto create_parser() -> parsing::ArgumentParser;



int main(int argc, char** argv) {
  auto parser = create_parser();
  auto args = parser.parse_args(argc - 1, argv + 1);
  show_args(args);
  return 0;
}


void show_args(std::unordered_map<std::string, parsing::Result> args) {
  bool banner_shown = false;

  std::ostringstream oss;
  oss << "\x1b[4mParsed Arguments\x1b[0m" << '\n';

  for (auto& [key,res] : args) {
    if (not banner_shown) {
      std::cout << oss.str();
      banner_shown = true;
    }
    std::cout << key << '\n';
    for (auto& value : res.values) {
      std::cout << "  " << value << '\n';
    }
  }
}


auto create_parser() -> parsing::ArgumentParser {
  parsing::ArgumentParser parser("hacky");
  parser.add_help(false);

  auto& positional_group = parser.add_argument_group("Positional");
  positional_group.add_argument("sources")
      .nargs("+")
      .help("Path(s) to SOURCE directories to check for file duplicates in.");

  auto& inner_group = parser.add_argument_group("General");
  inner_group.add_argument({"--threads", "-t"})
      .default_value("1")
      .help("How many threads to use.");
  inner_group.add_argument({"--recursive", "-r"})
      .action(parsing::actions::store_true)
      .help("Walk all subdirectories of SOURCES.");
  inner_group.add_argument({"--noempty", "--skip-empty"})
      .action(parsing::actions::store_true)
      .help("Skip empty files (all empty files hash to the same value, so it's worth skipping them. This may default to true in the future.).");

  inner_group.add_argument({"--si", "--binary"})
      .action(parsing::actions::store_true)
      .help("Use binary prefixes (KiB, MiB, etc.) instead of the default (KB, MB, etc.).");
  inner_group.add_argument({"--zero"})
      .dest("separator")
      .default_value("\n")
      .const_value("\0")
      .action(parsing::actions::store_const)
      .help("Use a null-terminator instead of newline to separate files and groups in the output.");

  auto& output_group = parser.add_argument_group("Output");
  output_group.add_argument({"--quiet", "-q"})
      .action(parsing::actions::store_true)
      .help("Don't display the files (still displays anything else that is normally displayed).");
  output_group.add_argument({"--silent", "-s"})
      .action(parsing::actions::store_true)
      .help("Show no output.");
  output_group.add_argument({"--loglevel"})
      .dest("loglevel")
      .default_value("30")
      .help("Adjust the logging level manually.");
  output_group.add_argument({"--debug"})
      .dest("loglevel")
      .action(parsing::actions::store_const)
      .const_value("10")
      .help("Show all output.");

  auto& info_group = parser.add_argument_group("Informational");
  info_group.add_argument({"--progress"})
      .action(parsing::actions::store_true)
      .help("Show a helpful progress bar instead of the nothing that currently gets shown.");
  info_group.add_argument({"--timed"})
      .action(parsing::actions::store_true)
      .help("Show elapsed time from the moment parsing has finished to the moment the program is done doing its work.");
  info_group.add_argument({"--wasted", "--wasted-space"})
      .action(parsing::actions::store_true)
      .help("Show total_hashed space taken up by duplicate files (not including the unique one).");
  info_group.add_argument({"--help", "-h"})
      .action(parsing::actions::help)
      .help("Show this help menu and then exit.");

  return parser;
}
