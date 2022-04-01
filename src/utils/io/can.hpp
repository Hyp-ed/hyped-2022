#pragma once

#include <cstdint>
#include <vector>

#include <utils/concurrent/lock.hpp>
#include <utils/concurrent/thread.hpp>
#include <utils/utils.hpp>
#include <memory>

namespace hyped {
namespace utils {
namespace io {

namespace can {

struct Frame {
  static constexpr uint32_t kExtendedMask = 0x80000000U;
  uint32_t id;
  bool extended;
  uint8_t len;
  uint8_t data[8];
};

}  // namespace can

class ICanProcessor {
 public:
  /**
   * @brief To be called by CAN receive side. Object processes received CAN
   * message and updates its local data
   *
   * @param message received CAN message to be processed
   */
  virtual void processNewData(can::Frame &message) = 0;

  /**
   * @brief To be called by CAN receive side to find owner of receinve can::Frame
   *
   * @param id        - of the received can::Frame
   * @param extended  - is the id extended?
   * @return true     - iff this CanProcessor owns the corresponding message
   */
  virtual bool hasId(uint32_t id, bool extended) = 0;
};

/**
 * Can implements singleton pattern to encapsulate one can interface, namely can0.
 * During object construction, can intereface is mapped onto socket_ member variable.
 * Furthermore, constructor spawns reading thread which waits on incoming can messages.
 * These messages are put into one of consuming queues based on configured id spaces.
 * The reading itself is performed in overriden run() method.
 */
class Can : public concurrent::Thread {
 public:
  static Can &getInstance()
  {
    static Can can;
    return can;
  }

  NO_COPY_ASSIGN(Can)

  /**
   * @param  frame data to be sent
   * @return 1     iff data sent successfully
   */
  int send(const can::Frame &frame);

  /**
   * @brief Called by any Can-enabled device implementing CanProcessor interface
   */
  void registerProcessor(ICanProcessor *processor);

  /**
   * @brief To be called for starting the receive thread
   */
  void start();

 private:
  /**
   * @param  frame output pointer to data to be filled
   * @return 1     iff data received successfully
   */
  int receive(can::Frame *frame);

  /**
   * @brief Process received message. Check whom does it belong to.
   * Send message to owner for processing.
   *
   * @param frame received CAN message
   */
  void processNewData(can::Frame *frame);

  /**
   * Blocking read and demultiplex messages based on configured id spaces
   */
  void run() override;

  Can();
  ~Can();

 private:
  int socket_;
  bool running_;
  std::vector<ICanProcessor *> processors_;
  concurrent::Lock socket_lock_;
};

}  // namespace io
}  // namespace utils
}  // namespace hyped
