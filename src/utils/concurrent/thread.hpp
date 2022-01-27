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

  /**
   * @brief      Spawn new thread and call Run() method
   */
  void start();

  /**
   * @brief      Wait until the thread terminates
   */
  void join();

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
  Logger log_;
};

class BusyThread : public Thread {
 public:
  ~BusyThread();
  void run() override;
  bool running_ = true;
};

}  // namespace concurrent
}  // namespace utils
}  // namespace hyped
