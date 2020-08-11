#include "util.hpp"

std::vector<std::string>
util::splitString(std::string target, std::string pattern)
{
  using namespace std;
  regex re(pattern);
  vector<std::string> tokens;

  copy(sregex_token_iterator(begin(target), end(target), re, -1), sregex_token_iterator(), back_inserter(tokens));

  return tokens;
}

bool
util::stringStartsWith(std::string target, std::string prefix)
{
  return (target.substr(0, prefix.size()) == prefix);
}
