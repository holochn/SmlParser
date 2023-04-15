#pragma once

// for c++ 20
// #include <source_location>
#include <stdio.h>

enum SmlLogLevel { Verbose, Debug, Info, Warning, Error };

class SmlLogger {
private:
  static SmlLogLevel logLevel;

  template <typename... Args>
  static void printLogMessage(const char *logLevelText, const char *message,
                              Args... args) {
    printf("[%s]\t", logLevelText);
    // for c++ 20:
    // printf("file %s ", location.file_name());
    // printf("function %s ", location.function_name());
    // printf("in line %d ", location.line());
    printf("%s", message, args...);
    printf("\n");
  }

public:
  static void setSmlLogLevel(SmlLogLevel newLogLevel) {
    logLevel = newLogLevel;
  }

  template <typename... Args>
  static void Verbose(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Verbose) {
      printLogMessage("Verbose", message, args...);
    }
  }

  template <typename... Args>
  static void Debug(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Debug) {
      printLogMessage("Debug", message, args...);
    }
  }

  template <typename... Args>
  static void Info(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Info) {
      printLogMessage("Info", message, args...);
    }
  }

  template <typename... Args>
  static void Warning(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Warning) {
      printLogMessage("Warning", message, args...);
    }
  }

  template <typename... Args>
  static void Error(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Error) {
      printLogMessage("Error", message, args...);
    }
  }
};