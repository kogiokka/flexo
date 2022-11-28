#ifndef RVL_LOG_P_H
#define RVL_LOG_P_H

#include "detail/rvl_p.h"

typedef struct RVLLog RVLLog;

#define RVL_LOG_LEVEL_TRACE 0x0000
#define RVL_LOG_LEVEL_DEBUG 0x0001
#define RVL_LOG_LEVEL_INFO  0x0002
#define RVL_LOG_LEVEL_WARN  0x0003
#define RVL_LOG_LEVEL_ERROR 0x0004
#define RVL_LOG_LEVEL_FATAL 0x0005

void rvl_log (RVLenum level, const char *funcName, const char *fmt, ...);

#ifndef NDEBUG
#define DEBUG_LOG(...) rvl_log (__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#define rvl_log_trace(...)                                                    \
  DEBUG_LOG (RVL_LOG_LEVEL_TRACE, __func__, __VA_ARGS__)
#define rvl_log_debug(...)                                                    \
  DEBUG_LOG (RVL_LOG_LEVEL_DEBUG, __func__, __VA_ARGS__)

#define rvl_log_info(...)  rvl_log (RVL_LOG_LEVEL_INFO, __func__, __VA_ARGS__)
#define rvl_log_warn(...)  rvl_log (RVL_LOG_LEVEL_WARN, __func__, __VA_ARGS__)
#define rvl_log_error(...) rvl_log (RVL_LOG_LEVEL_ERROR, __func__, __VA_ARGS__)
#define rvl_log_fatal(...) rvl_log (RVL_LOG_LEVEL_FATAL, __func__, __VA_ARGS__)

void rvl_log_set_level (RVLenum level);

#endif
