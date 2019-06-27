#include "utils.hh"
#include "gtest/gtest.h"

namespace {

TEST(has_prefix, basic) {
    EXPECT_TRUE(has_prefix("a", "abc"));
    EXPECT_TRUE(has_prefix("", ""));
    EXPECT_FALSE(has_prefix("a", "bbc"));
}

}
