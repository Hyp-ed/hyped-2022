#pragma once

#include <cstdint>

namespace hyped {
namespace data {

template<typename T>
class DataPoint {
 public:
  DataPoint() {}

  /*
   * Initialises this DataPoint with the specified timestamp and value.
   */
  DataPoint(uint32_t timestamp, const T &value) : timestamp(timestamp), value(value) {}

  uint32_t timestamp;
  T value;
};

}  // namespace data
}  // namespace hyped
