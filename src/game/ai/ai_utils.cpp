#include "ai_utils.h"

#include <sstream>

namespace ai_utils {

std::vector<std::string> extractSubstrings(std::string str) {
    if (str[0] != '(') {
        throw std::invalid_argument("AI condition parse error, missing \'(\'");
    }

    std::vector<std::string> ret_vec;
    std::string::iterator start_it = str.begin();
    int paren_count = 0;

    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (*it == '(') {
            if (paren_count == 0) {
                start_it = it;
            }
            paren_count++;
        } else if (*it == ')') {
            paren_count--;
            if (paren_count == 0) {
                ret_vec.push_back(std::string(start_it + 1, it));
            }
        }
    }

    if (paren_count != 0) {
        throw std::invalid_argument("AI condition parse error, unmatched parentheses");
    }

    return ret_vec;
}

std::vector<std::string> tokenizeString(std::string str) {
    std::vector<std::string> ret_vec;

    std::stringstream ss{str};

    std::string word;
    while (std::getline(ss, word, ' ')) {
        ret_vec.push_back(word);
    }

    return ret_vec;
}

} // ai_utils
