#include "variables.h"

bool PhysicsVariables::popJumps() {
    if (aerial_jumps_.available > 0) {
        aerial_jumps_.available--;
        return true;
    } else {
        return false;
    }
}

void PhysicsVariables::resetJumps() {
    aerial_jumps_.available = aerial_jumps_.max;
}

void PhysicsVariables::setMaxJumps(int jumps) {
    aerial_jumps_.max = jumps;
    resetJumps();
}

bool PhysicsVariables::popDashes() {
    if (dashes_.available > 0) {
        dashes_.available--;
        return true;
    } else {
        return false;
    }
}

void PhysicsVariables::resetDashes() {
    dashes_.available = dashes_.max;
}

void PhysicsVariables::setMaxDashes(int dashes) {
    dashes_.max = dashes;
    resetDashes();
}
