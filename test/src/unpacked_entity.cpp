#include <gtest/gtest.h>

#include "entity_editor/components/state_editor.h"
#include "entity_editor/components/animation_editor.h"
#include "utils/file.h"

#include "utils/log.h"

class UnpackedEntityTestFixture : public ::testing::Test {
  protected:
    const std::string entity_name_ = "player";
};

TEST_F(UnpackedEntityTestFixture, UnpackRepackAnimation) {
    auto dir_scope = File::pushDirectory(File::getEntityDir() / entity_name_);

    auto unpacked_animation = entity_editor::AnimationEditor();

    auto j1 = File::loadAnimations().value();
    auto texture = std::make_shared<sf::Texture>(File::loadTexture("Player.png").value());
    unpacked_animation.unpack(j1, texture);

    auto j2 = unpacked_animation.repack();

    EXPECT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST_F(UnpackedEntityTestFixture, UnpackRepackState) {
    auto dir_scope = File::pushDirectory(File::getEntityDir() / entity_name_);

    auto j1 = File::loadStates().value();

    auto unpacked_state = entity_editor::StateEditor();
    unpacked_state.unpack(j1);
    auto j2 = unpacked_state.repack();

    EXPECT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}
