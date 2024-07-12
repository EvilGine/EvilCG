#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecg_api.h>
#include <iostream>

TEST(group_ecg, test_ecg) {
    int res = test_function();
    std::cout << "Result: " << res << std::endl;
    EXPECT_EQ(0, res);
}