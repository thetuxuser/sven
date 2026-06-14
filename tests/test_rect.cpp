/**
 * test_rect.cpp — Unit tests for Sven::Rect
 *
 * Rect is a small aggregate struct used for sprite-sheet cropping.
 * These tests just confirm its shape and default values — there's no
 * SDL involved.
 */

#include "sven_test.h"
#include <sven.h>

using Sven::Rect;

TEST(Rect_DefaultConstructor_IsZero) {
    Rect r;
    EXPECT_EQ(r.x, 0);
    EXPECT_EQ(r.y, 0);
    EXPECT_EQ(r.width, 0);
    EXPECT_EQ(r.height, 0);
}

TEST(Rect_AggregateInitialization) {
    Rect r = {10, 20, 32, 64};
    EXPECT_EQ(r.x, 10);
    EXPECT_EQ(r.y, 20);
    EXPECT_EQ(r.width, 32);
    EXPECT_EQ(r.height, 64);
}

TEST(Rect_SpriteSheetFrameMath) {
    // A common pattern: picking the Nth frame of a horizontal strip of
    // 32x32 tiles.
    const int frameSize = 32;

    auto frameAt = [&](int index) -> Rect {
        return Rect{index * frameSize, 0, frameSize, frameSize};
    };

    Rect frame0 = frameAt(0);
    Rect frame2 = frameAt(2);

    EXPECT_EQ(frame0.x, 0);
    EXPECT_EQ(frame2.x, 64);

    EXPECT_EQ(frame0.width,  frameSize);
    EXPECT_EQ(frame2.height, frameSize);
}

TEST(Rect_FieldsAreIndependentlyMutable) {
    Rect r = {0, 0, 16, 16};

    r.x += 16; // advance to the next frame in a strip
    EXPECT_EQ(r.x, 16);
    EXPECT_EQ(r.width, 16); // unaffected
}

int main() {
    return Sven::Test::runAll();
}
