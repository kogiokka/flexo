#include "util.hpp"

std::vector<std::string>
util::str::split(std::string const& target, std::string const& pattern)
{
  using namespace std;
  regex re(pattern);
  vector<std::string> tokens;

  copy(sregex_token_iterator(begin(target), end(target), re, -1), sregex_token_iterator(), back_inserter(tokens));

  return tokens;
}

bool
util::str::startsWith(std::string const& target, std::string const& prefix)
{
  return (target.substr(0, prefix.size()) == prefix);
}
