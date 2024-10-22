#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define ENABLE_ECG_CL
#include <ecg_api.h>

TEST(ecg_api, init_ecg) {
	ecg::ecg_host_ctrl& host_ctrl = ecg::ecg_host_ctrl::get_instance();
	auto queue = host_ctrl.get_cmd_queue();
	auto context = host_ctrl.get_context();
	auto device = host_ctrl.get_device();

	ASSERT_NE(device, cl::Device());
	ASSERT_NE(context, cl::Context());
	ASSERT_NE(queue, cl::CommandQueue());
}

TEST(ecg_api, summ_vertexes) {
	ecg::mesh_t mesh;
}

TEST(ecg_api, get_center) {
	ecg::vec3_base result_center;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::mesh_t mesh;

	result_center = ecg::get_center(nullptr, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);

	result_center = ecg::get_center(&mesh, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
}

TEST(ecg_api, compute_aabb) {
	bool compare_result = false;
	ecg::bounding_box result_bb;
	ecg::ecg_status status;
	ecg::mesh_t mesh;

	result_bb = ecg::compute_aabb(nullptr, &status);
	compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);

	result_bb = ecg::compute_aabb(&mesh, &status);
	compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
}

TEST(ecg_api, compute_obb) {
	ecg::mesh_t mesh;
	ecg::compute_obb(&mesh);
}