#pragma once

#include <stdio.h>

#include <cstdint>

#include <utils/concurrent/lock.hpp>

namespace hyped::utils {

class Logger {
 public:
  enum class Level { kError, kInfo, kDebug };

  explicit Logger(const char *const module, const Level level);
  explicit Logger(const char *const module);

  void setLevel(const Level level);

  /**
   * @brief All debug messages have the same format. The arguments closely
   * follow the format-string signature of printf function.
   *
   * INFO is printed iff verbose_ for this logger set.
   * Output goes to STDOUT
   *
   * @param format - format string as in printf
   * @param ...    - arguments for the format string
   */
  void info(const char *format, ...) const;

  /**
   * @brief Use to indicate error. Output goes to STDERR
   */
  void error(const char *format, ...) const;

  /**
   * @brief Use for infrequent debug messages:
   * e.g. state transitions, successful initialisation
   * DBG is printed iff debug_ >= 0
   */
  void debug(const char *format, ...) const;

 private:
  const char *const module_;
  Level level_;
  static concurrent::Lock output_lock_;
  static void print(FILE *file, const char *format, va_list args);
  void printHead(FILE *file, const char *title) const;
};

}  // namespace hyped::utils
