/*
 * Author: Luis Kao
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util/Logger.h"

#define LOG_LEVEL_LAST LOG_LEVEL_FATAL

typedef struct logger logger;
typedef void (*logfn) (logger *self);

struct logger
{
  enum loglevel level;

  char        timestamp[80];
  const char *fn_name;
  const char *format;
  va_list     args;

  logfn write_fn;
};

static void logger_fwrite_default (logger *self);
static void logger_create_timestamp (logger *self);

static logger   the_logger    = { .write_fn = logger_fwrite_default };
static enum loglevel level_default = LOG_LEVEL_TRACE;

static int level_colors[LOG_LEVEL_LAST + 1] = { 94, 34, 93, 33, 31, 41 };
static int level_attrs[LOG_LEVEL_LAST + 1]  = { 2, 1, 1, 1, 1, 1 };
static const char *level_strings[LOG_LEVEL_LAST + 1] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

void
log_event (enum loglevel level, const char *fn_name, const char *fmt, ...)
{
  if (level < level_default)
    {
      return;
    }

  the_logger.level   = level;
  the_logger.fn_name = fn_name;
  the_logger.format  = fmt;

  va_start (the_logger.args, fmt);
  the_logger.write_fn (&the_logger);
  va_end (the_logger.args);
}

void
log_set_level (enum loglevel level)
{
  level_default = level;
}

void
logger_fwrite_default (logger *self)
{
  char  buf[512];
  char *dynbuf = NULL;

  char *out = buf;

  logger_create_timestamp (self);

  // Calculate the required lengths
  size_t hdrlen = snprintf (
      NULL, 0, "%s \033[%d;%dm%s\033[0m \033[90m%s:\033[0m ", self->timestamp,
      level_colors[self->level], level_attrs[self->level],
      level_strings[self->level], self->fn_name);

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
            level_colors[self->level], level_attrs[self->level],
            level_strings[self->level], self->fn_name);
  vsnprintf (out + hdrlen, msglen + 1, self->format, self->args);

  if (dynbuf != NULL)
    {
      log_warn ("The log message from \"%s\" was %lu bytes long, exceeding "
                "the buffer size.",
                self->fn_name, logsz);
    }

  fprintf (stdout, "%s\n", out);
  fflush (stdout);

  if (dynbuf != NULL)
    {
      free (dynbuf);
    }
}

void
logger_create_timestamp (logger *self)
{
  char *buf = self->timestamp;

  time_t tt;
  time (&tt);
  struct tm *now = gmtime (&tt);

  buf[strftime (buf, sizeof (self->timestamp), "%Y-%m-%dT%H:%M:%SZ", now)]
      = '\0';
}
