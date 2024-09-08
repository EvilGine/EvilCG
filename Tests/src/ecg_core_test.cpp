#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecg_api.h>

TEST(core_ecg, check_mesh) {
    ecg::ecg_status status;
	ecg::mesh_t mesh = {};

	status = ecg::check_mesh(nullptr);
	EXPECT_EQ(status, ecg::ARG_NULL_PTR);

	status = ecg::check_mesh(&mesh);
	EXPECT_NE(status, ecg::SUCCESS);
}

TEST(core_ecg, get_devices) {
    ecg::ecg_status status;

	status = ecg::get_devices();
	EXPECT_EQ(status, ecg::SUCCESS);
}