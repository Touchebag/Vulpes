#include <gtest/gtest.h>

#include "move.h"
#include "hitbox.h"
#include "collision.h"
#include "history.h"
#include "command_test.h"
#include "world_save.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
