#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecg_api.h>

TEST(compute_ecg, calculate_normals) {
	ecg::ecg_status status;
	ecg::mesh_t mesh = {};

	status = ecg::compute::calculate_normals(nullptr);
	EXPECT_EQ(status, ecg::ARG_NULL_PTR);

	status = ecg::compute::calculate_normals(&mesh);
	EXPECT_NE(status, ecg::SUCCESS);
}