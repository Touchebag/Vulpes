#pragma once

#include <vector>
#include <string>

#include "instructions.h"

namespace ai_utils {

std::pair<std::vector<int>, ai::InstructionData> loadInstructions(std::vector<std::string> str);

std::vector<std::string> tokenizeString(std::string str);

} // ai_utils
