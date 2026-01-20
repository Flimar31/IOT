#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdarg.h>

#define USE_ANSI 1

#if USE_ANSI
  #define ANSI_RESET   "\033[0m"
  #define ANSI_BOLD    "\033[1m"
  #define ANSI_RED     "\033[31m"
  #define ANSI_GREEN   "\033[32m"
  #define ANSI_YELLOW  "\033[33m"
  #define ANSI_BLUE    "\033[34m"
  #define ANSI_MAGENTA "\033[35m"
  #define ANSI_CYAN    "\033[36m"
  #define ANSI_WHITE   "\033[37m"
#else
  #define ANSI_RESET   ""
  #define ANSI_BOLD    ""
  #define ANSI_RED     ""
  #define ANSI_GREEN   ""
  #define ANSI_YELLOW  ""
  #define ANSI_BLUE    ""
  #define ANSI_MAGENTA ""
  #define ANSI_CYAN    ""
  #define ANSI_WHITE   ""
#endif

struct Logger {
  void info(const char* fmt, ...);
  void debug(const char* fmt, ...);
  void error(const char* fmt, ...);
  void success(const char* fmt, ...);
  void logColor(const char* colorCode, const char* fmt, ...);
  void separator();
private:
  void vprint(const char* level, const char* prefixColor, const char* msgColor, const char* fmt, va_list ap);
  void ts(char* out, size_t len);
};

extern Logger logger;

#endif // LOGGER_H
