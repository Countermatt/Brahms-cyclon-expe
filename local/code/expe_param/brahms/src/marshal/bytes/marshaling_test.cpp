#include <catch.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "marshal/bytes/marshaling.hpp"
#include "utils/string.hpp"
#include "utils/time.hpp"

using namespace bytes;

TEST_CASE( "Un/marshaling Bools with buffer", "[marshal][bytes][bool]" ) {
  bool falseInput = false, falseOutput;
  bool trueInput = true, trueOutput;
  char *buf;

  auto bufSize = MarshalBool(&buf, trueInput);
  UnmarshalBool(&trueOutput, buf, bufSize);
  delete[] buf;

  bufSize = MarshalBool(&buf, falseInput);
  UnmarshalBool(&falseOutput, buf, bufSize);
  delete[] buf;

  REQUIRE( trueInput == trueOutput );
  REQUIRE( falseInput == falseOutput );
}

TEST_CASE( "Un/marshaling Uint32 with buffer", "[marshal][bytes][uint32]" ) {
  uint32_t input = 123083, output;
  char *buf;

  auto bufSize = MarshalUint32(&buf, input);
  UnmarshalUint32(&output, buf, bufSize);
  delete[] buf;

  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling Ulong with buffer", "[marshal][bytes][ulong]" ) {
  unsigned long input = 1230830004756, output;
  char *buf;

  auto bufSize = MarshalUlong(&buf, input);
  UnmarshalUlong(&output, buf, bufSize);
  delete[] buf;

  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling Double with buffer", "[marshal][bytes][double]" ) {
  double input = 123083.78954132454, output;
  char *buf;

  auto bufSize = MarshalDouble(&buf, input);
  UnmarshalDouble(&output, buf, bufSize);
  delete[] buf;

  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling String with buffer", "[marshal][bytes][string]" ) {
  std::string input, output;
  char* buf;

  for (int i = 0; i < 10; i++) {
    input += "lalkqdhlkaluzkqsdjlldkj ";
  }

  auto bufSize = MarshalString(&buf, input);
  UnmarshalString(&output, buf, bufSize);
  delete[] buf;

  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling Time with buffer", "[marshal][bytes][time]" ) {
  Time input = Now(), output;
  char* buf;

  int inputSize = MarshalTime(&buf, input);
  int outputSize = UnmarshalTime(&output, buf, inputSize);
  delete[] buf;

  REQUIRE( inputSize == outputSize );
  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling string list with buffer", "[marshal][bytes][string_list]" ) {
  const int N_ITEMS = 3, ITEM_SIZE = 20;
  std::vector<std::string> input, output;
  char* buf;

  input.resize(N_ITEMS);
  for (int i = 0; i < N_ITEMS; i++)
    input[i] = randString(ITEM_SIZE);


  int inputSize = MarshalStringList(&buf, input, ';');
  int outputSize = UnmarshalStringList(output, buf, inputSize, ';');
  delete[] buf;

  REQUIRE( inputSize == outputSize );
  REQUIRE( input == output );
  // REQUIRE( input.size() == output.size() );
  // REQUIRE( std::equal(input.begin(), input.end(), output.begin()) );
}

TEST_CASE( "Un/marshaling empty string list", "[marshal][bytes][empty][string_list]" ) {
  std::vector<std::string> input, output;
  char* buf;

  input.clear();

  int inputSize = MarshalStringList(&buf, input, ';');
  int outputSize = UnmarshalStringList(output, buf, inputSize, ';');
  delete[] buf;
  
  REQUIRE( inputSize == outputSize );
  REQUIRE( input == output );
}

TEST_CASE( "Un/marshaling bytes buffer", "[marshal][bytes][bytes_buffer]" ) {
  char* input = (char*) "Imagine all the people";
  unsigned int inputSize = std::strlen(input), outputSize = 0;
  char* output = nullptr;
  char* buf;


  unsigned int bytesWritten = MarshalBytes(&buf, input, inputSize);
  unsigned int bytesRead = UnmarshalBytes(&output, &outputSize, buf, bytesWritten);
  delete[] buf;
  
  REQUIRE( bytesWritten == bytesRead );
  REQUIRE( inputSize == outputSize );
  REQUIRE( std::strncmp(input, output, inputSize) == 0 );
  delete[] output;
}

TEST_CASE( "Un/marshaling empty bytes buffer", "[marshal][bytes][empty][bytes_buffer]" ) {
  char* input = nullptr;
  unsigned int inputSize = 0, outputSize = 50;
  char* output = nullptr;
  char* buf;


  unsigned int bytesWritten = MarshalBytes(&buf, input, inputSize);
  unsigned int bytesRead = UnmarshalBytes(&output, &outputSize, buf, bytesWritten);
  delete[] buf;
  
  REQUIRE( bytesWritten == bytesRead );
  REQUIRE( inputSize == outputSize );
  REQUIRE( output == nullptr );
}