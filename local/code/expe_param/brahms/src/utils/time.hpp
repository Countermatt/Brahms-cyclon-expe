#ifndef UTILS_TIME_HPP_
#define UTILS_TIME_HPP_

#include <cstdint> // Partial SGX support?
#include <string>

#define TIME_MEM_SIZE 8 // Size in memory (see marhsal)

// Most implementations of time_t are uint64_t: seconds since UNIX epoch
// STL: "Portable programs should not use the value returned by this function directly, 
// but always rely on calls to other elements of the standard library
// to translate them to portable types (such as localtime, gmtime or difftime)."
// Well we can't :(
typedef uint64_t Time;
typedef uint64_t Duration;

// Returns current time as seconds
Time Now();

std::string TimeToString(Time t);

// bool operator<(const Time& t1, const Time& t2);
// bool operator>(const Time& t1, const Time& t2) { return t2 < t1; }
// bool operator<=(const Time& t1, const Time& t2) { return !(t1 > t2); }
// bool operator>=(const Time& t1, const Time& t2) { return !(t1 < t2); }
// Time operator+(const Time& t, const Duration& d);
// Time operator-(const Time& t, const Duration& d);
// Time operator%(const Time& t, const Duration& d);

// Duration operator*(const Duration& d, const unsigned int& x);
// Duration operator*(const unsigned int& x, const Duration& d);

#endif // UTILS_TIME_HPP_