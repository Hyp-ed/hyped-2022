#pragma once

#include <unordered_set>

#include <utils/io/can.hpp>
#include <utils/logger.hpp>

namespace hyped::debugging {

class CanListener : public utils::io::ICanProcessor {
 public:
  CanListener();
  void processNewData(utils::io::can::Frame &message) override;
  bool hasId(uint32_t id, bool extended) override;
  void subscribe(const uint32_t id);
  void unsubscribe(const uint32_t id);

 private:
  utils::Logger log_;
  std::unordered_set<uint32_t> ids_;
};

}  // namespace hyped::debugging
