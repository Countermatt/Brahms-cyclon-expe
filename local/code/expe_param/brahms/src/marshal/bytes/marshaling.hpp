#ifndef MARSHAL_BYTES_MARSHALING_HPP_
#define MARSHAL_BYTES_MARSHALING_HPP_

// Imported by enclave
// TODO: change types? 

#include <cstdint> // SGX "Partial" support
// #include <ctime> // SGX unsupported: clock(), mktime(), time(), ctime(), gmtime(), localtime()
#include <string> // full SGX support
#include <vector> // full SGX support

#include "utils/time.hpp"

// Advice on reading/writing simple types in binary format:
// https://faq.cprogramming.com/cgi-bin/smartfaq.cgi?id=1044780608&answer=1108255660

namespace bytes {

// Marhal* functions instanciate *buf without checks
// Unmarshal* functions throw runtime_error if buffer too short 
// All functions return number of bytes written/read

unsigned int MarshalBool(char** buf, const bool val);
unsigned int UnmarshalBool(bool* val, const char* buf, const unsigned int bufSize);

unsigned int MarshalUint32(char** buf, const uint32_t val);
unsigned int UnmarshalUint32(uint32_t* val, const char* buf, const unsigned int bufSize);

unsigned int MarshalUlong(char** buf, const unsigned long val);
unsigned int UnmarshalUlong(unsigned long* val, const char* buf, const unsigned int bufSize);

unsigned int MarshalDouble(char** buf, const double val);
unsigned int UnmarshalDouble(double* val, const char* buf, const unsigned int bufSize);

unsigned int MarshalString(char** buf, const std::string &str);
unsigned int UnmarshalString(std::string* str, const char* buf, const unsigned int bufSize);

unsigned int MarshalBytes(char** buf, const char* val, const unsigned int valSize);
unsigned int UnmarshalBytes(char** val, uint32_t* valSize, const char* buf, const unsigned int bufSize);

unsigned int MarshalTime(char** buf, const Time &val);
unsigned int UnmarshalTime(Time* val, const char* buf, const unsigned int bufSize);

// Marshaling only puts the list, unmarshaling needs the buf size
unsigned int MarshalStringList(
  char** buf, const std::vector<std::string> &l, 
  char sep);
unsigned int UnmarshalStringList(
  std::vector<std::string>& l, const char* buf, 
  const unsigned int bufSize, char sep);

} // namespace bytes

#endif // MARSHAL_BYTES_MARSHALING_HPP_