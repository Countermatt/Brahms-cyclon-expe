#ifndef UTILS_STRING_HPP_
#define UTILS_STRING_HPP_

// Imported by enclave

#include <string>
#include <vector>

// Copies splits of input string into l.
void splitString(std::vector<std::string> &l, const std::string& s, char sep);
void splitString(std::vector<std::string> &l, const char* buf, const int bufSize, char sep);
// Replace first occurence of <from> inside <str> to <to>.
bool replaceString(std::string& str, const std::string& from, const std::string& to);
// Rand string of size n
std::string randString(const int &n);

std::string stringToHex(const char* input, const unsigned int inputSize);
std::string stringToHex(const std::string& input);

#endif // UTILS_STRING_HPP_
