#pragma once

/**
 * sven_test.h — Tiny Test Framework
 *
 * A deliberately minimal test helper for Sven's own unit tests. No external
 * dependencies (no GoogleTest/Catch2) — keeps the test suite as simple and
 * readable as the library itself.
 *
 * Usage:
 *
 *   #include "sven_test.h"
 *
 *   TEST(MyFeature_DoesThing) {
 *       EXPECT_EQ(1 + 1, 2);
 *       EXPECT_TRUE(true);
 *       EXPECT_NEAR(3.14159f, 3.14f, 0.01f);
 *   }
 *
 *   int main() { return Sven::Test::runAll(); }
 */

#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace Sven::Test {

struct Case {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> cases;
    return cases;
}

/** Thrown by EXPECT_* macros on failure; caught by runAll(). */
struct Failure {
    std::string message;
};

// Registers a test case at static-init time.
struct Registrar {
    Registrar(const std::string& name, std::function<void()> fn) {
        registry().push_back({name, std::move(fn)});
    }
};

/** Runs every registered test, prints a summary, returns an exit code. */
inline int runAll() {
    int passed = 0;
    int failed = 0;

    for (const auto& test : registry()) {
        try {
            test.fn();
            std::printf("[ PASS ] %s\n", test.name.c_str());
            ++passed;
        } catch (const Failure& f) {
            std::printf("[ FAIL ] %s — %s\n", test.name.c_str(), f.message.c_str());
            ++failed;
        } catch (const std::exception& e) {
            std::printf("[ FAIL ] %s — unexpected exception: %s\n",
                         test.name.c_str(), e.what());
            ++failed;
        }
    }

    std::printf("\n%d passed, %d failed, %d total\n",
                 passed, failed, passed + failed);

    return failed == 0 ? 0 : 1;
}

} // namespace Sven::Test

// ─── Macros ─────────────────────────────────────────────────────────────────

#define SVEN_TEST_CONCAT_INNER(a, b) a##b
#define SVEN_TEST_CONCAT(a, b) SVEN_TEST_CONCAT_INNER(a, b)

#define TEST(name)                                                            \
    static void SVEN_TEST_CONCAT(sven_test_fn_, name)();                      \
    static ::Sven::Test::Registrar SVEN_TEST_CONCAT(sven_test_reg_, name)(    \
        #name, &SVEN_TEST_CONCAT(sven_test_fn_, name));                       \
    static void SVEN_TEST_CONCAT(sven_test_fn_, name)()

#define EXPECT_TRUE(cond)                                                      \
    do {                                                                       \
        if (!(cond)) {                                                        \
            throw ::Sven::Test::Failure{                                      \
                std::string("EXPECT_TRUE failed: ") + #cond                   \
                + " (" + __FILE__ + ":" + std::to_string(__LINE__) + ")"};    \
        }                                                                      \
    } while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

#define EXPECT_EQ(a, b)                                                        \
    do {                                                                       \
        if (!((a) == (b))) {                                                  \
            throw ::Sven::Test::Failure{                                      \
                std::string("EXPECT_EQ failed: ") + #a + " == " + #b          \
                + " (" + __FILE__ + ":" + std::to_string(__LINE__) + ")"};    \
        }                                                                      \
    } while (0)

#define EXPECT_NEAR(a, b, eps)                                                 \
    do {                                                                       \
        if (std::fabs((a) - (b)) > (eps)) {                                  \
            throw ::Sven::Test::Failure{                                      \
                std::string("EXPECT_NEAR failed: ") + #a + " ~= " + #b        \
                + " (" + __FILE__ + ":" + std::to_string(__LINE__) + ")"};    \
        }                                                                      \
    } while (0)
