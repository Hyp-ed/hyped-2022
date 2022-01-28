#include "system.hpp"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <csignal>
#include <cstring>

namespace hyped::utils {

/**
 * Static logger to be used before the system has been intialised and the logging
 * configuration is known.
 */
static Logger kInitialisationErrorLogger("SYSTEM", Logger::Level::kError);

System::System(const Config &config) : config_(config), log_("SYSTEM", config.log_level)
{
}

void System::parseArgs(int argc, char *argv[])
{
}

System &System::getSystem()
{
  if (system_) { return *system_; }
  kInitialisationErrorLogger.error(
    "somebody tried to access System before initialisation, aborting");
  exit(1);
}

Logger &System::getLogger()
{
  if (!system_) {
    kInitialisationErrorLogger.error(
      "somebody tried to access the system logger before initialisation, aborting");
    exit(1);
  }
  return system_->log_;
}

void System::interruptHandler(int)
{
  if (system_) { system_->stop(); }
  kInitialisationErrorLogger.info("interrupt signal received, exiting gracefully");
  exit(0);
}

void System::segfaultHandler(int)
{
  if (system_) { system_->stop(); }
  kInitialisationErrorLogger.error("forced termination detected (segfault?)");
  exit(1);
}

bool System::setExitFunction()
{
  static bool signal_set = false;
  if (signal_set) { return true; }

  // nominal termination
  std::signal(SIGINT, &interruptHandler);

  // forced termination
  std::signal(SIGSEGV, &segfaultHandler);
  std::signal(SIGABRT, &segfaultHandler);
  std::signal(SIGFPE, &segfaultHandler);
  std::signal(SIGILL, &segfaultHandler);
  std::signal(SIGTERM, &segfaultHandler);

  signal_set = true;
  return true;
}

bool System::isRunning()
{
  utils::concurrent::ScopedLock scoped_lock(&lock_);
  return running_;
}

void System::stop()
{
  utils::concurrent::ScopedLock scoped_lock(&lock_);
  running_ = false;
}

void System::start()
{
  utils::concurrent::ScopedLock scoped_lock(&lock_);
  running_ = true;
}

}  // namespace hyped::utils
