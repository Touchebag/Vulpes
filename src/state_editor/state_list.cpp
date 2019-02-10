#include "state_list.h"

StateList& StateList::getInstance() {
    static StateList state_list;

    return state_list;
}

std::vector<StateObject>& StateList::getObjects() {
    return objects_;
}
