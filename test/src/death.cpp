#include <gtest/gtest.h>

#include "components/death.h"
#include "base_entity.h"
#include "system/system.h"

class DeathTestFixture : public ::testing::Test {
  public:
    std::string entity_json_ = R"--(
{
    "Death": null,
    "Damageable": {
        "health": 6
    },
    "Collision": {
        "type": "hurtbox",
        "height": 50,
        "width": 50,
        "teams": [
            2
        ]
    },
    "Transform": {
        "pos_x": 100,
        "pos_y": 100
    }
}
        )--";

    std::string attack_json_ = R"--(
{
    "Collision": {
        "type": "hitbox",
        "height": 50,
        "width": 50,
        "attack": {
            "damage": 3
        },
        "teams": [
            1
        ]
    },
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
        )--";

    std::shared_ptr<BaseEntity> entity_;
    std::shared_ptr<BaseEntity> attack_;
};

TEST_F(DeathTestFixture, TestHealthDeath) {
    entity_ = BaseEntity::createFromJson(nlohmann::json::parse(entity_json_));
    attack_ = BaseEntity::createFromJson(nlohmann::json::parse(attack_json_));

    System::IWorldModify::addEntity(entity_);
    System::IWorldModify::addEntity(attack_);

    // Attack is not overlapping
    entity_->update();
    entity_->update();

    // Entity should not be dead
    EXPECT_FALSE(entity_->getComponent<Death>()->isDead());

    attack_->getComponent<Transform>()->setPosition(100, 100);

    entity_->update();
    entity_->update();

    // Now overlapping, should take 2 damage
    EXPECT_TRUE(entity_->getComponent<Death>()->isDead());
}

TEST_F(DeathTestFixture, TestHealthSameTeam) {
    auto ent = nlohmann::json::parse(entity_json_);
    auto atk = nlohmann::json::parse(attack_json_);

    // Change hurtbox to same team
    ent["Collision"]["teams"] = std::vector<int>{1};

    entity_ = BaseEntity::createFromJson(ent);
    attack_ = BaseEntity::createFromJson(atk);

    System::IWorldModify::addEntity(entity_);
    System::IWorldModify::addEntity(attack_);

    // Attack is not overlapping
    entity_->update();
    entity_->update();

    // Entity should not be dead
    EXPECT_FALSE(entity_->getComponent<Death>()->isDead());

    attack_->getComponent<Transform>()->setPosition(100, 100);

    entity_->update();
    entity_->update();

    // Now overlapping, should not be dead due to same team
    EXPECT_FALSE(entity_->getComponent<Death>()->isDead());
}
