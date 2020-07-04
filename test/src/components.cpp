#include <gtest/gtest.h>

#include "base_entity.h"

#include "components/collision/collision_static.h"
#include "components/collision/collision_player_hitbox.h"
#include "components/collision/collision_player_hurtbox.h"
#include "components/collision/collision_enemy_hitbox.h"
#include "components/collision/collision_transition.h"
#include "components/collision/collision_health.h"
#include "components/damageable/damageable_player.h"

nlohmann::json entity_json = nlohmann::json::parse(R"--(
{
    "Actions": null,
    "Animated": {
        "frame_list": [
            "Idle0",
            "Idle9"
        ],
        "sprite_map": "Player.txt"
    },
    "AI": {
        "file": "basic_enemy.json"
    },
    "Collision": {
        "type": "player_hitbox",
        "height": 200,
        "width": 50,
        "attack": {
            "damage": 2,
            "knockback_x": 15,
            "knockback_y": -5,
            "invincibility": 10
        }
    },
    "Damageable": {
        "health": 100
    },
    "Death": null,
    "Movable": null,
    "Physics": {
        "ground_acceleration": 7.0,
        "ground_friction": 0.7,
        "air_acceleration": 2.0,
        "air_friction": 0.93,
        "gravity": 1.0,
        "fall_multiplier": 3.5,
        "low_jump_multiplier": 2.5,
        "max_vertical_speed": 20.0,
        "min_vertical_speed": -80.0,
        "jump_impulse": -30.0,
        "wall_slide_friction": 0.85,
        "wall_jump_horizontal_impulse": 30.0,
        "wall_jump_vertical_impulse": -25.0,
        "dash_speed": 100.0,
        "dash_friction": 0.8
    },
    "Renderable": {
        "height": 200,
        "width": 50,
        "texture": "Player.png",
        "tile_x": true,
        "tile_y": true,
        "layer": "main"
    },
    "Stateful": {
        "states": "player_state.json"
    },
    "Subentity": null,
    "Transform": {
        "pos_x": 350,
        "pos_y": 1300
    }
}
)--");

TEST(TestComponents, TestLoadSaveAllComponents) {
    std::shared_ptr<BaseEntity> entity = BaseEntity::createFromJson(entity_json);

    auto e1 = entity->outputToJson();
    ASSERT_TRUE(e1.has_value());

    nlohmann::json j1 = e1.value();

    EXPECT_TRUE(entity_json == j1) << entity_json.dump() << std::endl << j1.dump() << std::endl;

    entity->reloadFromJson(j1);

    auto e2 = entity->outputToJson();
    ASSERT_TRUE(e2.has_value());

    nlohmann::json j2 = e2.value();

    EXPECT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadStatic) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");
    nlohmann::json attack = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "static";
    std::shared_ptr<CollisionStatic> coll =
        std::dynamic_pointer_cast<CollisionStatic>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerHurtbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");

    j1["type"] = "player_hurtbox";
    std::shared_ptr<CollisionPlayerHurtbox> coll =
        std::dynamic_pointer_cast<CollisionPlayerHurtbox>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerHitbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");
    j1["attack"] = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "player_hitbox";
    std::shared_ptr<CollisionPlayerHitbox> coll =
        std::dynamic_pointer_cast<CollisionPlayerHitbox>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    EXPECT_EQ(5, coll->getAttributes().damage);
    EXPECT_EQ(50, coll->getAttributes().knockback_x);
    EXPECT_EQ(10, coll->getAttributes().knockback_y);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadEnemyHitbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");
    j1["attack"] = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "enemy_hitbox";
    std::shared_ptr<CollisionEnemyHitbox> coll =
        std::dynamic_pointer_cast<CollisionEnemyHitbox>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    EXPECT_EQ(5, coll->getAttributes().damage);
    EXPECT_EQ(50, coll->getAttributes().knockback_x);
    EXPECT_EQ(10, coll->getAttributes().knockback_y);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadTransition) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");
    j1["destination"] = nlohmann::json::parse("{ \"room\": \"world.json\", \"entrance_id\": 1 }");

    j1["type"] = "transition";
    std::shared_ptr<CollisionTransition> coll =
        std::dynamic_pointer_cast<CollisionTransition>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadHealthCollectible) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    nlohmann::json j1= nlohmann::json::parse("{ \"width\": 50, \"height\": 50 }");
    j1["health"] = 10;

    j1["type"] = "health";
    std::shared_ptr<CollisionHealth> coll =
        std::dynamic_pointer_cast<CollisionHealth>(Collision::createFromJson(j1, trans));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerDamageable) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Collision> coll = std::make_shared<CollisionPlayerHurtbox>(trans);
    std::shared_ptr<MovableEntity> move = std::make_shared<MovableEntity>(trans, coll);
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Subentity> subent = std::make_shared<Subentity>(trans, move);
    std::shared_ptr<RenderableEntity> render = std::make_shared<RenderableEntity>(trans, move);
    std::shared_ptr<AnimatedEntity> anim = std::make_shared<AnimatedEntity>(render);
    std::shared_ptr<StatefulEntity> state = std::make_shared<StatefulEntity>(anim, subent);

    nlohmann::json j1;

    j1["health"] = 10;
    j1["type"] = "player";

    std::shared_ptr<DamageablePlayer> dmg =
        std::dynamic_pointer_cast<DamageablePlayer>(Damageable::createFromJson(j1, coll, death, state, render, move));
    // Ensure dynamic cast valid
    ASSERT_TRUE(dmg);

    nlohmann::json j2 = dmg->outputToJson().value();
    ASSERT_TRUE(j1== j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}
