#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define ENABLE_ECG_CL
#include <ecg_meshes.h>
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
	auto& meshes_inst = ecg_meshes::get_instance();
	ecg::vec3_base result_center;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::mesh_t mesh;

	result_center = ecg::summ_vertexes(nullptr, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);

	result_center = ecg::summ_vertexes(&mesh, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);

	for (auto& item : meshes_inst.template_meshes) {
		result_center = ecg::summ_vertexes(&item.mesh, &status);
		compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		ASSERT_FALSE(compare_result);
	}
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

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		result_center = ecg::get_center(&mesh_inst.loaded_meshes[mesh_id].mesh, &status);
		compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
	}
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

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_bb = ecg::compute_aabb(&item.mesh, &status);
		compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
		ASSERT_EQ(status, ecg::status_code::SUCCESS);

		auto obj_save_path = item.full_path.replace_extension("").string() + "_test_aabb.obj";
		ecg_meshes::save_bb_to_obj(&result_bb, obj_save_path);
	}
}

TEST(ecg_api, compute_obb) {
	ecg::full_bounding_box result_bb;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::mesh_t mesh;

	result_bb = ecg::compute_obb(nullptr, &status);
	compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);

	result_bb = ecg::compute_obb(&mesh, &status);
	compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_bb = ecg::compute_obb(&item.mesh, &status);
		compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
		ASSERT_EQ(status, ecg::status_code::SUCCESS);

		auto obj_save_path = item.full_path.replace_extension("").string() + "_test_obb.obj";
		ecg_meshes::save_bb_to_obj(&result_bb, obj_save_path);
	}
}

TEST(ecg_api, compute_surface_area) {
	float result_surf_area = 0.0f;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::mesh_t mesh;

	result_surf_area = ecg::compute_surface_area(nullptr, &status);
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	mesh.indexes_size = 4;
	mesh.indexes = new uint32_t[4];
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	mesh.vertexes_size = 4;
	mesh.vertexes = new ecg::vec3_base[4];
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	ASSERT_EQ(status, ecg::status_code::NOT_TRIANGULATED_MESH);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_surf_area = ecg::compute_surface_area(&item.mesh, &status);
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		ASSERT_NE(result_surf_area, -FLT_MAX);
	}
}