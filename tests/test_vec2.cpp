/**
 * test_vec2.cpp — Unit tests for Sven::Vec2
 *
 * Pure math, no SDL involved — these tests don't need a Window.
 */

#include "sven_test.h"
#include <sven.h>

using Sven::Vec2;

TEST(Vec2_DefaultConstructor_IsZero) {
    Vec2 v;
    EXPECT_EQ(v.x, 0.0f);
    EXPECT_EQ(v.y, 0.0f);
    EXPECT_TRUE(v == Vec2::Zero());
}

TEST(Vec2_Constructor_SetsComponents) {
    Vec2 v(3.0f, 4.0f);
    EXPECT_EQ(v.x, 3.0f);
    EXPECT_EQ(v.y, 4.0f);
}

TEST(Vec2_Addition) {
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    Vec2 c = a + b;
    EXPECT_EQ(c.x, 4.0f);
    EXPECT_EQ(c.y, 6.0f);
}

TEST(Vec2_Subtraction) {
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    Vec2 c = a - b;
    EXPECT_EQ(c.x, 3.0f);
    EXPECT_EQ(c.y, 4.0f);
}

TEST(Vec2_ScalarMultiplication) {
    Vec2 a(2.0f, 3.0f);
    Vec2 b = a * 2.0f;
    EXPECT_EQ(b.x, 4.0f);
    EXPECT_EQ(b.y, 6.0f);
}

TEST(Vec2_ScalarDivision) {
    Vec2 a(10.0f, 20.0f);
    Vec2 b = a / 2.0f;
    EXPECT_EQ(b.x, 5.0f);
    EXPECT_EQ(b.y, 10.0f);
}

TEST(Vec2_CompoundAssignment) {
    Vec2 a(1.0f, 1.0f);

    a += Vec2(2.0f, 3.0f);
    EXPECT_EQ(a.x, 3.0f);
    EXPECT_EQ(a.y, 4.0f);

    a -= Vec2(1.0f, 1.0f);
    EXPECT_EQ(a.x, 2.0f);
    EXPECT_EQ(a.y, 3.0f);

    a *= 2.0f;
    EXPECT_EQ(a.x, 4.0f);
    EXPECT_EQ(a.y, 6.0f);
}

TEST(Vec2_Equality) {
    Vec2 a(1.0f, 2.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c(1.0f, 3.0f);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(Vec2_Length) {
    Vec2 v(3.0f, 4.0f); // classic 3-4-5 triangle
    EXPECT_NEAR(v.length(), 5.0f, 0.0001f);

    EXPECT_EQ(Vec2::Zero().length(), 0.0f);
}

TEST(Vec2_Normalized) {
    Vec2 v(3.0f, 4.0f);
    Vec2 n = v.normalized();

    EXPECT_NEAR(n.length(), 1.0f, 0.0001f);
    EXPECT_NEAR(n.x, 0.6f, 0.0001f);
    EXPECT_NEAR(n.y, 0.8f, 0.0001f);
}

TEST(Vec2_NormalizedZero_StaysZero) {
    // Normalizing the zero vector shouldn't divide by zero or crash —
    // it should just return zero.
    Vec2 n = Vec2::Zero().normalized();
    EXPECT_EQ(n.x, 0.0f);
    EXPECT_EQ(n.y, 0.0f);
}

int main() {
    return Sven::Test::runAll();
}
