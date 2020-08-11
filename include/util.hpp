#pragma once

#include <algorithm>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

namespace util
{
namespace str
{
std::vector<std::string>
split(std::string target, std::string pattern);

bool
startsWith(std::string target, std::string prefix);
}
}
