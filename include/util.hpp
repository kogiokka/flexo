#pragma once

#include <algorithm>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

namespace util
{
std::vector<std::string>
splitString(std::string target, std::string pattern);

bool
stringStartsWith(std::string target, std::string prefix);
}
