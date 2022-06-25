#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <glad/glad.h>

#include "common/Logger.hpp"

namespace Logger
{
    namespace details
    {
        std::string timestamp(std::string const& format)
        {
            using namespace std;
            auto const now = chrono::system_clock::now();
            auto const timeT = chrono::system_clock::to_time_t(now);
            struct tm const* const timeInfo = localtime(&timeT);

            std::ostringstream timeOutput;
            timeOutput << put_time(timeInfo, format.c_str());
            return timeOutput.str();
        }

        std::string formatString(char const* format, ...)
        {
            va_list args;
            va_start(args, format);
            int const sz = vsnprintf(nullptr, 0, format, args);
            std::string output(sz, '\0');
            va_start(args, format);
            vsnprintf(output.data(), output.size() + 1, format, args);
            va_end(args);
            return output;
        }
    }
}

namespace Logger::GL
{
    namespace details
    {
        using Type = GLenum;
        using Severity = GLenum;

        enum m_Type : GLenum {
            Type_Error = GL_DEBUG_TYPE_ERROR,
            Type_DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            Type_UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            Type_Other = GL_DEBUG_TYPE_OTHER,
            Type_Marker = GL_DEBUG_TYPE_MARKER,
            Type_Performance = GL_DEBUG_TYPE_PERFORMANCE,
            Type_Portability = GL_DEBUG_TYPE_PORTABILITY,
            Type_PopGroup = GL_DEBUG_TYPE_POP_GROUP,
            Type_PushGroup = GL_DEBUG_TYPE_PUSH_GROUP,
        };

        enum m_Severity : GLenum {
            Severity_High = GL_DEBUG_SEVERITY_HIGH,
            Severity_Medium = GL_DEBUG_SEVERITY_MEDIUM,
            Severity_Low = GL_DEBUG_SEVERITY_LOW,
            Severity_Notification = GL_DEBUG_SEVERITY_NOTIFICATION,
        };

        std::string typeName(Type type)
        {
            switch (type) {
            case Type_Error:
                return "Error";
            case Type_DeprecatedBehavior:
                return "Deprecated Behavior";
            case Type_UndefinedBehavior:
                return "Undefined Behavior";
            case Type_Performance:
                return "Performance";
            case Type_Portability:
                return "Portability";
            case Type_PopGroup:
                return "Pop Group";
            case Type_PushGroup:
                return "Push Group";
            case Type_Marker:
                return "Marker";
            case Type_Other:
                return "Other";
            default:
                return "Unknown";
            }
        }

        std::string severityName(Severity severity)
        {
            switch (severity) {
            case Severity_High:
                return "High";
            case Severity_Medium:
                return "Medium";
            case Severity_Low:
                return "Low";
            case Severity_Notification:
                return "Notification";
            default:
                return "Unknown";
            }
        }

        void debugMessage(GLenum type, GLenum severity, GLchar const* message)
        {
            using namespace std;
            cout << "[" << ::Logger::details::timestamp().c_str() << "] "
                 << "[Debug] [Type: " << typeName(type) << ", Severity: " << severityName(severity) << "] " << message;
            cout << endl;
        }
    }

    void registerGLDebugCallback()
    {
        glDebugMessageCallback(
            []([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id,
               [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length,
               [[maybe_unused]] GLchar const* message,
               [[maybe_unused]] void const* user_param) noexcept { details::debugMessage(type, severity, message); },
            nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
}
