#include "unpacked_state.h"

#include "utils/log.h"

UnpackedState::UnpackedState(std::string name, nlohmann::json j) {
    (void)j;
    this->name = name;
    this->old_name = name;
}
