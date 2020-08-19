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
split(std::string const& target, std::string const& pattern);

bool
startsWith(std::string const& target, std::string const& prefix);
}
}
