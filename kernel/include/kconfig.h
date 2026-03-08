#ifndef KERNEL_KCONFIG_H
#define KERNEL_KCONFIG_H

#ifndef KCONFIG_VERSION_MAJOR
  #error "KCONFIG_VERSION_MAJOR not defined"
#endif

#ifndef KCONFIG_VERSION_MINOR
  #error "KCONFIG_VERSION_MINOR not defined"
#endif

#ifndef KCONFIG_VERSION_PATCH
  #error "KCONFIG_VERSION_PATCH not defined"
#endif

#ifdef KCONFIG_LOG_LEVEL
  #ifndef KCONFIG_ENABLE_LOGGING
    #warning "KCONFIG_LOG_LEVEL but KCONFIG_ENABLE_LOGGING not set"
  #endif
#endif

#endif
