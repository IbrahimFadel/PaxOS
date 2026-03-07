#include "logging.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

static char log_buf[MAX_LOG_LEN + 10];

void log(log_level_t level, const char *fmt, ...) {
  char msg_buf[MAX_LOG_LEN];
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg_buf, MAX_LOG_LEN, fmt, args);
  va_end(args);

  const char *level_str = "UNKOWN";
  switch (level) {
  case LOG_LEVEL_ERROR:   level_str = "ERROR"; break;
  case LOG_LEVEL_WARNING: level_str = "WARN"; break;
  case LOG_LEVEL_TRACE:   level_str = "TRACE"; break;
  case LOG_LEVEL_DEBUG:   level_str = "DEBUG"; break;
  case LOG_LEVEL_INFO:    level_str = "INFO"; break;
  }

  // TODO: get timestamp

  int len = snprintf(log_buf, sizeof(log_buf), "[%s] %s", level_str, msg_buf);
  assert(len <= MAX_LOG_LEN + 10);
  printf("%s", log_buf);
}
