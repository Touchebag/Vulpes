#include <gtest/gtest.h>

#include "attributes/hitbox.h"

// TODO Move to collide when we can test collision between Collision objects directly

TEST(HitboxTests, DISABLED_CollideAtOrigin) {
    Hitbox h1, h2;

    h1.setHitbox(10, 20);
    h2.setHitbox(5, 30);

    // EXPECT_TRUE(h1.collides(h2));
}

TEST(HitboxTests, DISABLED_CollideAtOffset) {
    Hitbox h1, h2;

    h1.setHitbox(10, 20);
    h1.setOffset({5, 10});
    h2.setHitbox(20, 30);
    h2.setOffset({5, 20});

    // EXPECT_TRUE(h1.collides(h2));
}

TEST(HitboxTests, DISABLED_NotColliding) {
    Hitbox h1, h2;

    h1.setHitbox(10, 10);
    h1.setOffset({20, 50});
    h2.setHitbox(10, 10);
    h2.setOffset({-40, 10});

    // EXPECT_FALSE(h1.collides(h2));
}

TEST(HitboxTests, DISABLED_NotCollidingTouching) {
    Hitbox h1, h2;

    h1.setHitbox(10, 10);
    h1.setOffset({5, 5});
    h2.setHitbox(10, 10);
    h2.setOffset({-5, -5});

    // EXPECT_FALSE(h1.collides(h2));
}
