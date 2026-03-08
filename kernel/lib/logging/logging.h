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

#define LOG_DISCARD_(FMT, ...) ((void)sizeof(FMT, ##__VA_ARGS__))

#ifdef KCONFIG_ENABLE_LOGGING
  #define LOGE(FMT, ...) log(LOG_LEVEL_ERROR, (FMT), ##__VA_ARGS__)

  #if KCONFIG_LOG_LEVEL > 0
    #define LOGW(FMT, ...) log(LOG_LEVEL_WARNING, (FMT), ##__VA_ARGS__)
  #else
    #define LOGW(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #endif

  #if KCONFIG_LOG_LEVEL > 1
    #define LOGT(FMT, ...) log(LOG_LEVEL_TRACE, (FMT), ##__VA_ARGS__)
  #else
    #define LOGT(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #endif

  #if KCONFIG_LOG_LEVEL > 2
    #define LOGD(FMT, ...) log(LOG_LEVEL_DEBUG, (FMT), ##__VA_ARGS__)
  #else
    #define LOGD(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #endif

  #if KCONFIG_LOG_LEVEL > 3
    #define LOGI(FMT, ...) log(LOG_LEVEL_INFO, (FMT), ##__VA_ARGS__)
  #else
    #define LOGI(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #endif
#else
  #define LOGE(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #define LOGW(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #define LOGT(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #define LOGD(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
  #define LOGI(FMT, ...) LOG_DISCARD_(FMT, ##__VA_ARGS__)
#endif

#endif
