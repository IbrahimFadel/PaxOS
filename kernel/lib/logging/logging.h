#ifndef KERNEL_LIB_LOGGING_H
#define KERNEL_LIB_LOGGING_H

#define MAX_LOG_LEN 256

typedef enum {
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
} log_level_t;

void log(log_level_t level, const char *fmt, ...);

#define LOGE(FMT, ...) log(LOG_LEVEL_ERROR, (FMT), ##__VA_ARGS__)
#define LOGW(FMT, ...) log(LOG_LEVEL_WARNING, (FMT), ##__VA_ARGS__)
#define LOGT(FMT, ...) log(LOG_LEVEL_TRACE, (FMT), ##__VA_ARGS__)
#define LOGD(FMT, ...) log(LOG_LEVEL_DEBUG, (FMT), ##__VA_ARGS__)
#define LOGI(FMT, ...) log(LOG_LEVEL_INFO, (FMT), ##__VA_ARGS__)

#endif
