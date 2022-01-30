#include "utils/logger.hpp"

#include <stdarg.h>

#include <chrono>
#include <ctime>
#include <unordered_map>

namespace hyped::utils {

std::optional<Logger::Level> Logger::levelFromInt(const int level)
{
  static const std::unordered_map<int, Level> kIntToLevel
    = {{0, Level::kError}, {1, Level::kInfo}, {2, Level::kDebug}};
  const auto it = kIntToLevel.find(level);
  if (it == kIntToLevel.end()) { return std::nullopt; }
  return it->second;
}

void Logger::print(FILE *file, const char *format, va_list args)
{
  vfprintf(file, format, args);
  fprintf(file, "\n");
}

void Logger::printHead(FILE *file, const char *title) const
{
  using namespace std::chrono;
  std::time_t t = std::time(nullptr);
  tm *tt        = localtime(&t);
  fprintf(file, "%02d:%02d:%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);

  static const bool print_micro = true;
  if (print_micro) {
    auto now_time                       = high_resolution_clock::now().time_since_epoch();
    duration<int, std::milli> time_span = duration_cast<std::chrono::milliseconds>(now_time);
    fprintf(file, ".%03d ", static_cast<uint16_t>(time_span.count()) % 1000);
  } else {
    fprintf(file, " ");
  }
  fprintf(file, "%s[%s]: ", title, module_);
}

Logger::Logger(const char *const module, const Level level) : module_(module), level_(level)
{
}

Logger::Logger(const char *const module) : module_(module), level_(Level::kInfo)
{
}

void Logger::setLevel(const Logger::Level level)
{
  level_ = level;
}

void Logger::error(const char *format, ...) const
{
  static FILE *file = stderr;
  utils::concurrent::ScopedLock scoped_lock(&output_lock_);
  printHead(file, "ERROR");
  va_list args;
  va_start(args, format);
  print(file, format, args);
  va_end(args);
}

void Logger::info(const char *format, ...) const
{
  static FILE *file = stdout;
  if (level_ == Level::kDebug || level_ == Level::kInfo) {
    utils::concurrent::ScopedLock scoped_lock(&output_lock_);
    printHead(file, "INFO");
    va_list args;
    va_start(args, format);
    print(file, format, args);
    va_end(args);
  }
}

void Logger::debug(const char *format, ...) const
{
  static FILE *file = stdout;
  if (level_ == Level::kDebug) {
    utils::concurrent::ScopedLock scoped_lock(&output_lock_);
    printHead(file, "DEBUG");
    va_list args;
    va_start(args, format);
    print(file, format, args);
    va_end(args);
  }
}

}  // namespace hyped::utils
