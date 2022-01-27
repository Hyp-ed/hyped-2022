#include "main.hpp"

#include <cstdint>

namespace hyped::state_machine {

Main::Main()
    : utils::concurrent::Thread(
      utils::Logger("STATE-MACHINE", utils::System::getSystem().config_.log_level_state_machine))
{
  current_state_ = Idle::getInstance();
}

void Main::run()
{
  utils::System &sys = utils::System::getSystem();
  data::Data &data   = data::Data::getInstance();

  current_state_->enter(log_);

  State *new_state;
  while (sys.isRunning()) {
    // checkTransition returns a new state or nullptr
    if ((new_state = current_state_->checkTransition(log_))) {
      current_state_->exit(log_);
      current_state_ = new_state;
      current_state_->enter(log_);
    }

    // Yielding because running the loop twice without any other thread being active
    // will result in identical behaviour and thus waste resources.
    yield();
  }

  data::StateMachine sm_data = data.getStateMachineData();
  const auto state_string    = ::hyped::data::stateToString(sm_data.current_state);
  log_.info("exiting. current state: %s", state_string->c_str());
}

}  // namespace hyped::state_machine
