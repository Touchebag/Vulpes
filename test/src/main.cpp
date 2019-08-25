#include <gtest/gtest.h>

#include "move.h"
#include "hitbox.h"
#include "collision.h"

#include "command_test.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
