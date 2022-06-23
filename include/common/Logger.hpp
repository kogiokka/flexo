#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

namespace Logger::details
{
    std::string timestamp(std::string const& format = "%Y-%m-%d %H:%M:%S");
    std::string formatString(char const* format, ...);
}

namespace Logger
{
    template <typename... Params>
    void info(char const* format, Params&&... args)
    {
        using namespace std;
        cout << "[" << details::timestamp().c_str() << "]"
             << " [Info] " << details::formatString(format, std::forward<Params>(args)...);
        cout << endl;
    }

    template <typename... Params>
    void error(char const* format, Params&&... args)
    {
        using namespace std;
        cout << "[" << details::timestamp().c_str() << "]"
             << " [Error] " << details::formatString(format, std::forward<Params>(args)...);
        cout << endl;
    }

    namespace GL
    {
        void registerGLDebugCallback();
    }
}

#endif
