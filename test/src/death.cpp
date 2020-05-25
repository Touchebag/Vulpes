#include <gtest/gtest.h>

#include "components/death.h"
#include "base_entity.h"

class DeathTestFixture : public ::testing::Test {
  public:
    DeathTestFixture() {
        entity_ = BaseEntity::createFromJson(nlohmann::json::parse(entity_json_));
        attack_ = BaseEntity::createFromJson(nlohmann::json::parse(attack_json_));
    }

    void SetUp() override {
    }

    std::string entity_json_ = R"--(
{
    "Death": null,
    "Damageable": {
        "health": 2
    },
    "Collision": {
        "type": "enemy_hitbox"
    },
    "Hitbox": {
        "height": 50,
        "width": 50
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
        "type": "player_hitbox"
    },
    "Hitbox": {
        "height": 50,
        "width": 50
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
    // Attack is not overlapping
    entity_->update();
    entity_->update();

    // Entity should not be dead
    EXPECT_FALSE(entity_->death_->isDead());

    attack_->trans_->setPosition(100, 100);

    entity_->update();
    entity_->update();

    // Now overlapping shoudl take 2 damage
    EXPECT_TRUE(entity_->death_->isDead);
}
