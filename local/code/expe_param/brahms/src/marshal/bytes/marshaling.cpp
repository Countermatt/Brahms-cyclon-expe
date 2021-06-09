#include "marshal/bytes/marshaling.hpp"

#include <cstring> // memcpy is supported
#include <stdexcept> // Full SGX support
#include "utils/string.hpp"

namespace bytes {

unsigned int MarshalBool(char** buf, const bool val) {
  *buf = new char[1];
  (*buf)[0] = char(val);
  return 1;
}
unsigned int UnmarshalBool(bool* val, const char* buf, const unsigned int bufSize) {
  if (bufSize < 1)
    throw std::runtime_error("Short read of bool");
  *val = bool(buf[0]);
  return 1;
}

unsigned int MarshalUint32(char** buf, const uint32_t val) {
  *buf = new char[4];
  memcpy(*buf, &val, 4);
  return 4;
}
unsigned int UnmarshalUint32(uint32_t* val, const char* buf, const unsigned int bufSize) {
  if (bufSize < 4)
    throw std::runtime_error("Short read of uint32");
  memcpy(val, buf, 4);
  return 4;
}

unsigned int MarshalUlong(char** buf, const unsigned long val) {
  *buf = new char[8];
  memcpy(*buf, &val, 8);
  return 8;
}
unsigned int UnmarshalUlong(unsigned long* val, const char* buf, const unsigned int bufSize) {
  if (bufSize < 8)
    throw std::runtime_error("Short read of ulong");
  memcpy(val, buf, 8);
  return 8;
}

unsigned int MarshalDouble(char** buf, const double val) {
  *buf = new char[8];
  memcpy(*buf, &val, 8);
  return 8;
}
unsigned int UnmarshalDouble(double* val, const char* buf, const unsigned int bufSize) {
  if (bufSize < 8)
    throw std::runtime_error("Short read of double");
  memcpy(val, buf, 8);
  return 8;
}


unsigned int MarshalString(char** buf, const std::string &str) {
  unsigned int strSize = str.size();
  unsigned int bufSize = 4 + strSize;
  *buf = new char[bufSize];
  memcpy(*buf, &strSize, 4);
  memcpy(*buf+4, str.c_str(), strSize);

  return bufSize;
}
unsigned int UnmarshalString(std::string* str, const char* buf, const unsigned int bufSize) {
  if (bufSize < 4)
    throw std::runtime_error("Short read of string size");
  uint32_t strSize;
  memcpy(&strSize, buf, 4);

  if (bufSize < 4+strSize)
    throw std::runtime_error("Short read of string");
  *str = std::string(buf+4, strSize);

  return 4 + strSize;
}

unsigned int MarshalBytes(char** buf, const char* val, const unsigned int valSize) {
  unsigned int bufSize = 4 + valSize;
  *buf = new char[bufSize];
  memcpy(*buf, &valSize, 4);
  memcpy(*buf+4, val, valSize);
  return bufSize;
}
unsigned int UnmarshalBytes(char** val, uint32_t* valSize, const char* buf, const unsigned int bufSize) {
  if (bufSize < 4)
    throw std::runtime_error("Short read of bytes buffer size");
  memcpy(valSize, buf, 4);
  if (bufSize < 4+*valSize)
    throw std::runtime_error("Short read of bytes buffer");
  if (*valSize != 0) {
    *val = new char[*valSize];
    memcpy(*val, buf+4, *valSize);
  }
  return 4+*valSize;
}

unsigned int MarshalTime(char** buf, const Time &val) {
  *buf = new char[TIME_MEM_SIZE];
  memcpy(*buf, &val, TIME_MEM_SIZE);
  return TIME_MEM_SIZE;

  // //*buf = new char[20];
  
  // // localtime not supported by SGX
  // strftime(*buf, 20, "%Y-%m-%dT%H:%M:%SZ", localtime(&val));

  // return 20;
}
unsigned int UnmarshalTime(Time* val, const char* buf, const unsigned int bufSize) {
  if (bufSize < TIME_MEM_SIZE)
    throw std::runtime_error("Short read of time");
  memcpy(val, buf, TIME_MEM_SIZE);
  return TIME_MEM_SIZE;
  // struct tm t;
  // const char* p = buf;

  // // Parse the datetime text
  // // Daylight saving time -- -1 for unavailable
  // t.tm_isdst = -1;
  // // YYYY
  // t.tm_year = atoi(p) - 1900;
  // p += 5;
  // // MM
  // t.tm_mon = atoi(p) - 1;
  // p += 3;
  // // DD
  // t.tm_mday = atoi(p);
  // p += 3;
  // // HH
  // t.tm_hour = atoi(p);
  // p += 3;
  // // mm
  // t.tm_min = atoi(p);
  // p += 3;
  // // ss
  // t.tm_sec = atoi(p); 

  // *val = mktime(&t);
  // return 20;
}

// Format:
// [[<String><SEP>]*<String>]?
unsigned int MarshalStringList(
  char** buf, const std::vector<std::string> &l, 
  char sep) {

  char* p;
  unsigned int bufSize = 0, nItems = l.size();
  
  // Compute output size, create output buffer
  // Start with number of separators: (nItems - 1) (or 0 if l is empty)
  if (!l.empty())
    bufSize += nItems - 1;
  for (auto &it : l)  
    bufSize += it.size();
  *buf = new char[bufSize];

  // Write each string into output buffer
  p = *buf;
  for (unsigned int i = 0; i < nItems; i++) {
    memcpy(p, l[i].c_str(), l[i].size());
    p += l[i].size();

    // Add separator
    if (i != nItems - 1)
      memcpy(p++, &sep, 1);
  }

  return bufSize;
}
unsigned int UnmarshalStringList(
  std::vector<std::string>& l, const char* buf, 
  const unsigned int bufSize, char sep) {

  splitString(l, buf, bufSize, sep);
  return bufSize;
}

} // namespace bytes