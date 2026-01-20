#include "logger.h"
#include "logger_ansi.h"

Logger logger;

void Logger::info(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint("INFO", ANSI_CYAN, NULL, fmt, ap);
    va_end(ap);
}

void Logger::debug(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint("DEBUG", ANSI_MAGENTA, NULL, fmt, ap);
    va_end(ap);
}

void Logger::error(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint("ERROR", ANSI_RED, ANSI_RED, fmt, ap);
    va_end(ap);
}

void Logger::success(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint("SUCCESS", ANSI_GREEN, ANSI_GREEN, fmt, ap);
    va_end(ap);
}

void Logger::logColor(const char* colorCode, const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint("INPUT", ANSI_WHITE, colorCode, fmt, ap);
    va_end(ap);
}

void Logger::separator() {
    Serial.print(ANSI_BOLD);
    Serial.println("----------------------------------------------------------");
    Serial.print(ANSI_RESET);
}

void Logger::vprint(const char* level, const char* prefixColor, const char* msgColor, const char* fmt, va_list ap) {
    char msg[256];
    vsnprintf(msg, sizeof(msg), fmt, ap);

    char timebuf[24];
    ts(timebuf, sizeof(timebuf));

    Serial.print("[");
    Serial.print(timebuf);
    Serial.print("] ");

    Serial.print(ANSI_BOLD);
    Serial.print(prefixColor);
    Serial.print("[");
    Serial.print(level);
    Serial.print("] ");
    Serial.print(ANSI_RESET);

    if (msgColor != NULL) {
        Serial.print(msgColor);
    }

    Serial.print(msg);
    Serial.print(ANSI_RESET);

    Serial.println();
}

void Logger::ts(char* out, size_t len) {
    unsigned long now = millis();
    unsigned long s = now / 1000;
    unsigned long ms = now % 1000;
    unsigned long mn = (s / 60) % 60;
    unsigned long hh = (s / 3600) % 24;
    s = s % 60;
    if (hh > 0) snprintf(out, len, "%02lu:%02lu:%02lu.%03lu", hh, mn, s, ms);
    else if (mn > 0) snprintf(out, len, "%02lu:%02lu.%03lu", mn, s, ms);
    else snprintf(out, len, "%02lu.%03lu", s, ms);
}
