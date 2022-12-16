/*
 * Author: Luis Kao
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */

#ifndef LOGGER_H
#define LOGGER_H

enum loglevel
{
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL,
};

#ifdef __cplusplus
extern "C"
{
#endif

void log_event (enum loglevel level, const char *fn_name, const char *fmt, ...);

#ifndef NDEBUG
#define DEBUG_LOG(...) log_event (__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#define log_trace(...) DEBUG_LOG (LOG_LEVEL_TRACE, __func__, __VA_ARGS__)
#define log_debug(...) DEBUG_LOG (LOG_LEVEL_DEBUG, __func__, __VA_ARGS__)

#define log_info(...)  log_event (LOG_LEVEL_INFO, __func__, __VA_ARGS__)
#define log_warn(...)  log_event (LOG_LEVEL_WARN, __func__, __VA_ARGS__)
#define log_error(...) log_event (LOG_LEVEL_ERROR, __func__, __VA_ARGS__)
#define log_fatal(...) log_event (LOG_LEVEL_FATAL, __func__, __VA_ARGS__)

void log_set_level (enum loglevel level);

#ifdef __cplusplus
}
#endif

#endif

