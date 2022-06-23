#ifndef UTIL_H
#define UTIL_H

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException()
        : std::logic_error("Not yet implemented") {};
};

namespace util
{
    namespace str
    {
        std::vector<std::string> split(std::string const& target, std::string const& pattern);

        bool startsWith(std::string const& target, std::string const& prefix);
    }
}

#endif
