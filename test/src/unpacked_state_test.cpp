#include <gtest/gtest.h>

#include "entity_editor/state_view.h"
#include "utils/file.h"

#include "utils/log.h"

class UnpackedStateTestFixture : public ::testing::Test {
  public:
    void SetUp() override {
    }

    void TearDown() override {
        if (File::getCurrentDirectory() != "") {
            File::popDirectory();
        }
    }

  protected:
    const std::string entity_name_ = "player";
};

TEST_F(UnpackedStateTestFixture, UnpackRepackState) {
    File::pushDirectory(File::getEntityDir() / entity_name_);

    auto j1 = File::loadStates().value();

    auto unpacked_state = StateView();
    unpacked_state.unpack(j1);
    auto j2 = unpacked_state.repack();

    EXPECT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}
