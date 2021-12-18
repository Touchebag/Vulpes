#include "state_handler.h"

#include "components/actions/actions.h"

#include "utils/log.h"

namespace {

// Merges source into destination, prioritising source if conflict
inline void mergeJson(nlohmann::json& source, nlohmann::json& destination) {
    for (auto entry = source.begin(); entry != source.end(); ++entry) {
        if (entry->type() == nlohmann::json::value_t::array) {
            // Merge lists
            for (auto it : entry.value()) {
                destination[entry.key()] += it;
            }
        } else if (entry->type() == nlohmann::json::value_t::object) {
            destination[entry.key()].update(entry.value());
        } else {
            destination[entry.key()] = entry.value();
        }
    }
}

}

template <class T>
void StateHandler<T>::resetState() {
    // TODO Error handling
    current_state_ = state_list_.at("main");
}

template <class T>
void StateHandler<T>::reloadFromJson(const nlohmann::json& j) {
    state_list_.clear();

    if (!j.contains("states")) {
        LOGE("Unable to load states");
        throw std::invalid_argument("Unable to load states");
    }

    std::unordered_map<std::string, nlohmann::json> template_map;

    if (j.contains("templates")) {
        for (auto templ = j["templates"].begin(); templ != j["templates"].end(); ++templ) {
            template_map.insert(std::make_pair(templ.key(), templ.value()));
        }
    }

    for (auto state = j["states"].begin(); state != j["states"].end(); ++state) {
        nlohmann::json output;
        auto st = state.value();

        if (st.contains("templates")) {
            auto template_list = st["templates"];
            for (auto template_string : template_list) {
                try {
                    auto template_state = template_map.at(template_string.get<std::string>());
                    mergeJson(template_state, output);
                } catch (std::out_of_range& e) {
                    LOGE("State template %s not found", st["template"].get<std::string>().c_str());
                    throw e;
                }
            }
        }

        mergeJson(st, output);

        state_list_.insert(std::make_pair(state.key(),
                    std::make_shared<State<T>>(
                        State<T>::loadStateFromJson(output))));
    }

    resetState();
}

template <class T>
std::weak_ptr<State<T>> StateHandler<T>::incomingEvent(state_utils::Event event) {
    std::optional<std::string> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        // TODO Error handling
        current_state_ = state_list_.find(new_state.value())->second;
        return current_state_;
    } else {
        return std::weak_ptr<State<T>>();
    }
}

template <class T>
const T& StateHandler<T>::getStateData() {
    return current_state_->getData();
}


template class StateHandler<state_utils::EntityContent>;
