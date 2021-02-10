#include <gtest/gtest.h>

#include "base_entity.h"

#include "components/actions/actions_player.h"

#include "components/collision/collideables/collideable_player_hurtbox.h"
#include "components/collision/collideables/collideable_transition.h"
#include "components/collision/collideables/collideable_health.h"
#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

#include "components/collision/collideables/movement/collideable_static.h"
#include "components/collision/collideables/movement/collideable_semisolid.h"
#include "components/collision/collideables/movement/collideable_slope.h"

#include "components/collision/collideables/damage/collideable_player_hitbox.h"
#include "components/collision/collideables/damage/collideable_enemy_hitbox.h"

#include "components/damageable/damageable_player.h"

nlohmann::json entity_json = nlohmann::json::parse(R"--(
{
    "Actions": null,
    "Animated": {
        "frame_list": [
            "Idle0",
            "Idle9"
        ]
    },
    "AI": null,
    "Collision": {
        "type": "player_hitbox",
        "height": 200,
        "width": 46,
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
    "Death": {
        "death_entity": {
            "Entity": "enemy.json"
        }
    },
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
        "max_air_dive_speed": 80.0,
        "jump_impulse": -30.0,
        "wall_slide_friction": 0.85,
        "wall_jump_horizontal_impulse": 30.0,
        "wall_jump_vertical_impulse": -25.0,
        "dash_speed": 100.0,
        "dash_friction": 0.8,
        "air_dive_multiplier": 5.0,
        "air_dive_impulse": -30.0
    },
    "Renderable": {
        "height": 200,
        "width": 100,
        "texture": "box.png",
        "tile_x": true,
        "tile_y": true,
        "layer": 0
    },
    "Stateful": null,
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
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");
    nlohmann::json attack = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "static";
    std::shared_ptr<CollideableStatic> coll =
        std::dynamic_pointer_cast<CollideableStatic>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerHurtbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");

    j1["type"] = "player_hurtbox";
    std::shared_ptr<CollideablePlayerHurtbox> coll =
        std::dynamic_pointer_cast<CollideablePlayerHurtbox>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerHitbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");
    j1["attack"] = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "player_hitbox";
    std::shared_ptr<CollideablePlayerHitbox> coll =
        std::dynamic_pointer_cast<CollideablePlayerHitbox>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    EXPECT_EQ(5, coll->getAttributes().damage);
    EXPECT_EQ(50, coll->getAttributes().knockback_x);
    EXPECT_EQ(10, coll->getAttributes().knockback_y);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadEnemyHitbox) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");
    j1["attack"] = nlohmann::json::parse("{ \"damage\": 5, \"knockback_x\": 50, \"knockback_y\": 10 }");

    j1["type"] = "enemy_hitbox";
    std::shared_ptr<CollideableEnemyHitbox> coll =
        std::dynamic_pointer_cast<CollideableEnemyHitbox>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    EXPECT_EQ(5, coll->getAttributes().damage);
    EXPECT_EQ(50, coll->getAttributes().knockback_x);
    EXPECT_EQ(10, coll->getAttributes().knockback_y);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadTransition) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");
    j1["destination"] = nlohmann::json::parse("{ \"room\": \"world.json\", \"entrance_id\": 1 }");

    j1["type"] = "transition";
    std::shared_ptr<CollideableTransition> coll =
        std::dynamic_pointer_cast<CollideableTransition>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadHealthCollectible) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");
    j1["health"] = 10;

    j1["type"] = "health";
    std::shared_ptr<CollideableHealth> coll =
        std::dynamic_pointer_cast<CollideableHealth>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerDamageable) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    std::shared_ptr<Collision> coll = std::make_shared<Collision>(trans, actions);
    std::shared_ptr<MovableEntity> move = std::make_shared<MovableEntity>(trans, coll);
    std::shared_ptr<Subentity> subent = std::make_shared<Subentity>(trans, move);
    std::shared_ptr<RenderableEntity> render = std::make_shared<RenderableEntity>(trans, move);
    std::shared_ptr<AnimatedEntity> anim = std::make_shared<AnimatedEntity>(render);
    std::shared_ptr<StatefulEntity> state = std::make_shared<StatefulEntity>(anim, subent, actions, coll);

    nlohmann::json j1;

    j1["health"] = 10;
    j1["type"] = "player";

    std::shared_ptr<DamageablePlayer> dmg =
        std::dynamic_pointer_cast<DamageablePlayer>(Damageable::createFromJson(j1, coll, death, state, render, move));
    // Ensure dynamic cast valid
    ASSERT_TRUE(dmg);

    nlohmann::json j2 = dmg->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadCollisionCollectible) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);

    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");

    j1["id"] = 2;
    j1["type"] = "collectible";

    std::shared_ptr<CollideableCollectible> coll =
        std::dynamic_pointer_cast<CollideableCollectible>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadCollisionSemiSolid) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);

    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");

    j1["type"] = "semi_solid";

    std::shared_ptr<CollideableSemiSolid> coll =
        std::dynamic_pointer_cast<CollideableSemiSolid>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadInteractable) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);

    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");

    j1["type"] = "interactable";
    j1["transition"]["name"] = "test";
    j1["transition"]["id"] = 6;

    std::shared_ptr<CollideableInteractable> coll =
        std::dynamic_pointer_cast<CollideableInteractable>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;

    // Test with cutscene
    j1["cutscene"] = "test_cutscene";

    coll = std::dynamic_pointer_cast<CollideableInteractable>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadSlope) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);

    nlohmann::json j1 = nlohmann::json::parse("{ \"width\": 46, \"height\": 52 }");

    j1["type"] = "slope";
    j1["direction"] = "left";

    std::shared_ptr<CollideableSlope> coll =
        std::dynamic_pointer_cast<CollideableSlope>(Collideable::createFromJson(j1, trans, actions));
    // Ensure dynamic cast valid
    ASSERT_TRUE(coll);

    nlohmann::json j2 = coll->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadPlayerActions) {
    std::shared_ptr<Death> death = std::make_shared<Death>();

    nlohmann::json j1;

    j1["type"] = "player";

    std::shared_ptr<ActionsPlayer> actions =
        std::dynamic_pointer_cast<ActionsPlayer>(Actions::createFromJson(j1, death));
    // Ensure dynamic cast valid
    ASSERT_TRUE(actions);

    nlohmann::json j2 = actions->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;

    j1["enabled_actions"]["movement_x"] = true;
    j1["enabled_actions"]["dash"] = true;
    j1["enabled_actions"]["jump"] = true;
    j1["enabled_actions"]["wall_jump"] = true;
    j1["enabled_actions"]["double_jump"] = true;
    j1["enabled_actions"]["attack"] = true;
    j1["enabled_actions"]["interact"] = true;

    actions = std::dynamic_pointer_cast<ActionsPlayer>(Actions::createFromJson(j1, death));
    // Ensure dynamic cast valid
    ASSERT_TRUE(actions) << "ActionsPlayer, cast failed" << std::endl;

    j2 = actions->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}

TEST(TestComponents, TestSaveLoadAnimationFile) {
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<Death> death = std::make_shared<Death>();
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
    std::shared_ptr<Collision> coll = std::make_shared<Collision>(trans, actions);
    std::shared_ptr<MovableEntity> move = std::make_shared<MovableEntity>(trans, coll);
    std::shared_ptr<RenderableEntity> render = std::make_shared<RenderableEntity>(trans, move);

    // When empty it will default to loading file
    nlohmann::json j1;

    std::shared_ptr<AnimatedEntity> anim = std::make_shared<AnimatedEntity>(render);;
    anim->reloadFromJson(j1);

    nlohmann::json j2 = anim->outputToJson().value();
    ASSERT_TRUE(j1 == j2) << j1.dump() << std::endl << j2.dump() << std::endl;
}
