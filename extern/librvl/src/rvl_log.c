#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "detail/rvl_log_p.h"

#define RVL_LOG_LEVEL_LAST RVL_LOG_LEVEL_FATAL

typedef void (*RVLLogFn) (RVLLog *self);

struct RVLLog
{
  RVLenum level;

  char        timestamp[80];
  const char *funcName;
  const char *format;
  va_list     args;

  RVLLogFn writeFn;
};

static void rvl_log_fwrite_default (RVLLog *self);
static void rvl_log_create_timestamp (RVLLog *self);

static RVLLog  LOGGER        = { .writeFn = rvl_log_fwrite_default };
static RVLenum LOGGING_LEVEL = RVL_LOG_LEVEL_TRACE;

static const char *levelStrings[RVL_LOG_LEVEL_LAST + 1] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

static int levelColors[RVL_LOG_LEVEL_LAST + 1] = { 94, 34, 93, 33, 31, 41 };
static int levelAttrs[RVL_LOG_LEVEL_LAST + 1]  = { 2, 1, 1, 1, 1, 1 };

void
rvl_log (RVLenum level, const char *funcName, const char *fmt, ...)
{
  if (level < LOGGING_LEVEL)
    {
      return;
    }

  LOGGER.level    = level;
  LOGGER.funcName = funcName;
  LOGGER.format   = fmt;

  va_start (LOGGER.args, fmt);
  LOGGER.writeFn (&LOGGER);
  va_end (LOGGER.args);
}

void
rvl_log_set_level (RVLenum level)
{
  LOGGING_LEVEL = level;
}

void
rvl_log_fwrite_default (RVLLog *self)
{
  char  buf[512];
  char *dynbuf = NULL;

  char *out = buf;

  rvl_log_create_timestamp (self);

  // Calculate the required lengths
  size_t hdrlen = snprintf (
      NULL, 0, "%s \033[%d;%dm%s\033[0m \033[90m%s:\033[0m ", self->timestamp,
      levelColors[self->level], levelAttrs[self->level],
      levelStrings[self->level], self->funcName);

  va_list args;
  va_copy (args, self->args);
  size_t msglen = vsnprintf (NULL, 0, self->format, args);
  va_end (args);

  size_t logsz = hdrlen + msglen + 1;
  if (logsz > sizeof (buf))
    {
      dynbuf = (char *)malloc (logsz);
      out    = dynbuf;
    }

  // Write to char arrays
  snprintf (out, hdrlen + 1,
            "%s \033[%d;%dm%s\033[0m \033[90m%s\033[0m: ", self->timestamp,
            levelColors[self->level], levelAttrs[self->level],
            levelStrings[self->level], self->funcName);
  vsnprintf (out + hdrlen, msglen + 1, self->format, self->args);

  if (dynbuf != NULL)
    {
      rvl_log_warn (
          "The log message from \"%s\" was %lu bytes long, exceeding "
          "the buffer size.",
          self->funcName, logsz);
    }

  fprintf (stdout, "%s\n", out);
  fflush (stdout);

  if (dynbuf != NULL)
    {
      free (dynbuf);
    }
}

void
rvl_log_create_timestamp (RVLLog *self)
{
  char *buf = self->timestamp;

  time_t tt;
  time (&tt);
  struct tm *now = gmtime (&tt);

  buf[strftime (buf, sizeof (self->timestamp), "%Y-%m-%dT%H:%M:%SZ", now)]
      = '\0';
}
