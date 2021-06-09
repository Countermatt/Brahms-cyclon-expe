#include <catch.hpp>

#include <string>
#include <vector>

#include "utils/string.hpp"

TEST_CASE("Splitting string into vector", "[utils][string_list][string]") {
  std::vector<std::string> input, output;
  input.push_back("azerty");
  input.push_back("qwerty");
  input.push_back("dvorak");
  input.push_back("youpi");
  std::string inputString("azerty\nqwerty\ndvorak\nyoupi");

  splitString(output, inputString, '\n');

  REQUIRE( input == output );
}

TEST_CASE("Splitting byte array into vector.", "[utils][string_list][bytes]") {
  std::vector<std::string> input, output;
  input.push_back("azerty");
  input.push_back("qwerty");
  input.push_back("dvorak");
  input.push_back("youpi");
  std::string inputString("azerty\nqwerty\ndvorak\nyoupi");

  splitString(output, inputString.c_str(), inputString.size(), '\n');

  REQUIRE( input == output );
}

TEST_CASE("Splitting string into vector with sep at the end", "[utils][string_list][string]") {
  std::vector<std::string> input, output;
  input.push_back("azerty");
  input.push_back("qwerty");
  input.push_back("dvorak");
  input.push_back("youpi");
  input.push_back("");
  std::string inputString("azerty\nqwerty\ndvorak\nyoupi\n");

  splitString(output, inputString, '\n');

  REQUIRE( input == output );
}

TEST_CASE("Splitting byte array into vector with sep at the end.", "[utils][string_list][bytes]") {
  std::vector<std::string> input, output;
  input.push_back("azerty");
  input.push_back("qwerty");
  input.push_back("dvorak");
  input.push_back("youpi");
  input.push_back("");
  std::string inputString("azerty\nqwerty\ndvorak\nyoupi\n");

  splitString(output, inputString.c_str(), inputString.size(), '\n');

  REQUIRE( input == output );
}