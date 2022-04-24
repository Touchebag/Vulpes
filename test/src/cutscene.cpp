#include <gtest/gtest.h>

#include "system/cutscene.h"
#include "system/system.h"

class CutsceneTestFixture : public ::testing::Test {
  public:
    CutsceneTestFixture() {
        nlohmann::json j;
        j["Entity"] = "cutscene_test_enemy";

        entity_ = BaseEntity::createFromJson(j);
        System::IWorldModify::addEntity(entity_);
    }

    ~CutsceneTestFixture() {
        entity_->getComponent<Death>()->setDead();
    }

    std::shared_ptr<BaseEntity> entity_;
};

TEST_F(CutsceneTestFixture, TestFadeOut) {
    nlohmann::json j = nlohmann::json::parse(R"--(
    {
        "events": {
            "0": [
                {
                    "event": "fade_out",
                    "entity": "cutscene_test",
                    "active_frames": 5
                }
            ]
        }
    }
    )--");

    auto cutscene = Cutscene();
    cutscene.reloadFromJson(j);
    cutscene.start();

    ASSERT_EQ(entity_->getComponent<Rendering>()->getColor().a, 255);

    cutscene.update();

    ASSERT_EQ(entity_->getComponent<Rendering>()->getColor().a, 204);
}
