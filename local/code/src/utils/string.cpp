#include "utils/string.hpp"
#include "utils/random.hpp"

void splitString(std::vector<std::string> &l, const std::string& s, char sep) {
  if (!l.empty())
    l.clear();
  if (s.empty())
    return;

  std::string item;
  int itemStartPos = 0;
  for (unsigned int i = 0; i < s.size(); i++) {
    if (s[i] == sep) {
      item = s.substr(itemStartPos, i - itemStartPos);
      l.push_back(item);
      itemStartPos = i+1;
    }
  }
  item = s.substr(itemStartPos, std::string::npos);
  l.push_back(item);
}

void splitString(
  std::vector<std::string> &l, const char* buf, 
  const int bufSize, char sep) {

  if (!l.empty())
    l.clear();
  if (bufSize == 0)
    return;

  std::string item;
  int itemStartPos = 0;
  for (int i = 0; i < bufSize; i++) {
    if (buf[i] == sep) {
      item = std::string(buf + itemStartPos, i - itemStartPos);
      l.push_back(item);
      itemStartPos = i+1;
    }
  }
  item = std::string(buf + itemStartPos, bufSize - itemStartPos);
  l.push_back(item);
}


std::string randString(const int &n) { 
  std::string res, alphabet = "abcdefghijklmnopqrstuvwxyz0123456789_";
  res.resize(n);
  for (int i = 0; i < n; i++) 
  {
    unsigned int r = random_int();
    res[i] = alphabet[r % alphabet.size()]; 
  }
  return res; 
}

bool replaceString(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::string stringToHex(const char* input, const unsigned int inputSize) {
  static const char* const lut = "0123456789abcdef";

    std::string output;
    output.reserve(2 * inputSize);
    for (size_t i = 0; i < inputSize; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
std::string stringToHex(const std::string& input) {
  return stringToHex(input.data(), input.size());
}