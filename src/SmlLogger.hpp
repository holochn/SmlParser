#pragma once

#include <stdio.h>
#include <string>

enum SmlLogLevel { Verbose, Debug, Info, Warning, Error };
const std::string SmlLogColor_White = "\033[0;37m";
const std::string SmlLogColor_Yellow = "\033[0;33m";
const std::string SmlLogColor_Red = "\033[0;31m";

class SmlLogger {
private:
  static SmlLogLevel logLevel;

  template <typename... Args>
  static void printLogMessage(const char *logLevelText, const char *message,
                              std::string logColor, Args... args) {

    std::string strAsFormat = logColor;
    strAsFormat += "[%s]\t";
    strAsFormat += message;
    strAsFormat += "\n";

    printf(strAsFormat.c_str(), logLevelText, args...);
  }

public:
  static void setSmlLogLevel(SmlLogLevel newLogLevel) {
    logLevel = newLogLevel;
  }

  template <typename... Args>
  static void Verbose(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Verbose) {
      printLogMessage("Verbose", message, SmlLogColor_White, args...);
    }
  }

  template <typename... Args>
  static void Debug(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Debug) {
      printLogMessage("Debug", message, SmlLogColor_White, args...);
    }
  }

  template <typename... Args>
  static void Info(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Info) {
      printLogMessage("Info", message, SmlLogColor_White, args...);
    }
  }

  template <typename... Args>
  static void Warning(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Warning) {
      printLogMessage("Warning", message, SmlLogColor_Yellow, args...);
    }
  }

  template <typename... Args>
  static void Error(const char *message, Args... args) {
    if (logLevel <= SmlLogLevel::Error) {
      printLogMessage("Error", message, SmlLogColor_Red, args...);
    }
  }
};