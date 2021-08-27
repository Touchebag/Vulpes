#include <vector>
#include <string>

namespace ai_utils {

std::vector<int> loadInstructions(std::string str);

std::vector<std::string> extractArguments(std::string str);

std::vector<std::string> tokenizeString(std::string str);

} // ai_utils
