#pragma once

#include <cstdint>
#include <thread>

#include <utils/logger.hpp>

namespace hyped {
namespace utils {
namespace concurrent {

class Thread {
 public:
  explicit Thread(Logger log);
  virtual ~Thread() {}

  /**
   * @brief      Spawn new thread and call Run() method
   */
  void start();

  /**
   * @brief      Wait until the thread terminates
   */
  void join();

  void stop();

  /**
   * @brief      Thread entry point
   */
  virtual void run();

  static void yield();

  uint8_t getId() { return id_; }

  static void sleep(uint32_t ms);

 private:
  inline static uint8_t next_id_;
  uint8_t id_;
  std::thread *thread_;

 protected:
  std::atomic<bool> is_running_ = true;
  Logger log_;
};

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
