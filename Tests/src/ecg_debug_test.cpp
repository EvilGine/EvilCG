#ifdef _DEBUG
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ecg_api.h>

TEST(debug_ecg, debug) {
	ecg::ecg_status status = ecg::ecg_debug_func();
	EXPECT_EQ(status, ecg::SUCCESS);
}
#endif