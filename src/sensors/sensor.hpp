#pragma once

#include <data/data.hpp>

namespace hyped::sensors {

class ISensor {
 public:
  /**
   * @brief Check if sensor is responding, i.e. connected to the system
   * @return true - if sensor is online
   */
  virtual bool isOnline() = 0;
};

class ICounter : public ISensor {
 public:
  /**
   * @brief empty virtual deconstructor for proper deletion of derived classes
   */
  virtual ~ICounter() {}

  /**
   * @brief Get GPIO data
   * @param stripe_counter - output pointer
   */
  virtual data::CounterData getData() = 0;
};

}  // namespace hyped::sensors
