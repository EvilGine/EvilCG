#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ecg_timer.h>
#define ENABLE_ECG_CL
#include <ecg_meshes.h>
#include <ecg_api.h>

TEST(ecg_api, init_ecg) {
	ecg::ecg_host_ctrl& host_ctrl = ecg::ecg_host_ctrl::get_instance();
	auto queue = host_ctrl.get_cmd_queue();
	auto context = host_ctrl.get_context();
	auto device = host_ctrl.get_device();

	std::cout << "Device Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

	ASSERT_NE(device, cl::Device());
	ASSERT_NE(context, cl::Context());
	ASSERT_NE(queue, cl::CommandQueue());
}

TEST(ecg_api, summ_vertexes) {
	auto& meshes_inst = ecg_meshes::get_instance();
	bool compare_result = false;
	uint32_t test_counter = 0;
	ecg::ecg_status status;
	custom_timer_t timer;
	ecg::ecg_mesh_t mesh;
	
	ecg::vec3_base gpu_result;
	ecg::vec3_base cpu_result;

	timer.start();
	gpu_result = ecg::summ_vertexes(nullptr, &status);
	compare_result = ecg::compare_vec3_base(gpu_result, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);
	timer.end();

#if defined(_DEBUG) && defined(SHOW_MESSAGES)
	std::cout << "[INF]:> #" << test_counter++ << " - " << timer << std::endl;
#endif

	timer.start();
	gpu_result = ecg::summ_vertexes(&mesh, &status);
	compare_result = ecg::compare_vec3_base(gpu_result, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
	timer.end();

#if defined(_DEBUG) && defined(SHOW_MESSAGES)
	std::cout << "[INF]:> #" << test_counter++ << " - " << timer << std::endl;
#endif

	auto cpu_check = [](ecg::vec3_base* vertexes, uint32_t vertexes_size) {
		ecg::vec3_base result;
		for (size_t i = 0; i < vertexes_size; ++i) {
			result = ecg::add_vec(result, vertexes[i]);
		}
		return result;
	};

	auto check_func = [&](std::vector<ecg_test_mesh_ptr>& meshes) {
		constexpr size_t vec_size = sizeof(ecg::vec3_base) / sizeof(float);
		for (auto& item : meshes) {
			timer.start();
			gpu_result = ecg::summ_vertexes(&item->mesh, &status);
			cpu_result = cpu_check(item->mesh.vertexes, item->mesh.vertexes_size);
			compare_result = ecg::compare_vec3_base(gpu_result, cpu_result);
			timer.end();

#if defined(_DEBUG) && defined(SHOW_MESSAGES)
			std::cout << "[INF]:> #" << test_counter++ << " - " << timer << "\n"
				<< "\tMesh name: " << item->full_path << "\n"
				<< "\tMesh vertexes cnt: " << item->mesh.vertexes_size << std::endl;
			std::cout << "Compare result: " << compare_result 
				<< "\n\tgpu: {" << gpu_result.x << " , " << gpu_result.y << " , " << gpu_result.z << " }" 
				<< "\n\tcpu: {" << cpu_result.x << " , " << cpu_result.y << " , " << cpu_result.z << " }" 
				<< std::endl;
#endif
		}
	};

	check_func(meshes_inst.template_meshes);
	check_func(meshes_inst.loaded_meshes);
}

TEST(ecg_api, get_center) {
	ecg::vec3_base result_center;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::ecg_mesh_t mesh;
	custom_timer_t timer;

	timer.start();
	result_center = ecg::get_center(nullptr, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);
	timer.end();

	timer.start();
	result_center = ecg::get_center(&mesh, &status);
	compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
	timer.end();

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		timer.start();
		result_center = ecg::get_center(&mesh_inst.loaded_meshes[mesh_id]->mesh, &status);
		compare_result = ecg::compare_vec3_base(result_center, ecg::vec3_base());
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		timer.end();
	}
}

TEST(ecg_api, compute_aabb) {
	bool compare_result = false;
	ecg::bounding_box result_bb;
	ecg::ecg_status status;
	ecg::ecg_mesh_t mesh;
	custom_timer_t timer;

	timer.start();
	result_bb = ecg::compute_aabb(nullptr, &status);
	compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);
	timer.end();

	timer.start();
	result_bb = ecg::compute_aabb(&mesh, &status);
	compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
	timer.end();

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		timer.start();
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_bb = ecg::compute_aabb(&item->mesh, &status);
		compare_result = ecg::compare_bounding_boxes(result_bb, ecg::default_bb);
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		timer.end();

		auto obj_save_path = item->full_path.replace_extension("").string() + "_test_aabb.obj";
		ecg_meshes::save_bb_to_obj(&result_bb, obj_save_path);
	}
}

TEST(ecg_api, compute_obb) {
	ecg::full_bounding_box result_bb;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::ecg_mesh_t mesh;
	custom_timer_t timer;

	timer.start();
	result_bb = ecg::compute_obb(nullptr, &status);
	compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);
	timer.end();

	timer.start();
	result_bb = ecg::compute_obb(&mesh, &status);
	compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
	timer.end();

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		timer.start();
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_bb = ecg::compute_obb(&item->mesh, &status);
		compare_result = ecg::compare_full_bb(result_bb, ecg::full_bounding_box());
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		timer.end();

		auto obj_save_path = item->full_path.replace_extension("").string() + "_test_obb.obj";
		ecg_meshes::save_bb_to_obj(&result_bb, obj_save_path);
	}
}

TEST(ecg_api, compute_surface_area) {
	float result_surf_area = 0.0f;
	bool compare_result = false;
	ecg::ecg_status status;
	ecg::ecg_mesh_t mesh;
	custom_timer_t timer;

	timer.start();
	result_surf_area = ecg::compute_surface_area(nullptr, &status);
	timer.end();
	
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	timer.start();
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	timer.end();
	
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	timer.start();
	mesh.vertexes_size = 4;
	mesh.vertexes = new ecg::vec3_base[4];
	result_surf_area = ecg::compute_surface_area(&mesh, &status);
	timer.end();
	
	ASSERT_EQ(status, ecg::status_code::NOT_TRIANGULATED_MESH);
	EXPECT_FLOAT_EQ(result_surf_area, -FLT_MAX);

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		timer.start();
		auto item = mesh_inst.loaded_meshes[mesh_id];
		result_surf_area = ecg::compute_surface_area(&item->mesh, &status);
		timer.end();

		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		ASSERT_NE(result_surf_area, -FLT_MAX);
	}
}

TEST(ecg_api, compute_covariance_matrix) {
	bool compare_result = false;
	ecg::mat3_base cov_matrix;
	ecg::ecg_status status;
	ecg::ecg_mesh_t mesh;
	custom_timer_t timer;

	timer.start();
	cov_matrix = ecg::compute_covariance_matrix(nullptr, &status);
	compare_result = ecg::compare_mat3(cov_matrix, ecg::null_mat3);
	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(compare_result);
	timer.end();

	timer.start();
	cov_matrix = ecg::compute_covariance_matrix(&mesh, &status);
	compare_result = ecg::compare_mat3(cov_matrix, ecg::null_mat3);
	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(compare_result);
	timer.end();

	auto& mesh_inst = ecg_meshes::get_instance();
	for (size_t mesh_id = 0; mesh_id < mesh_inst.loaded_meshes.size(); ++mesh_id) {
		timer.start();
		auto item = mesh_inst.loaded_meshes[mesh_id];
		cov_matrix = ecg::compute_covariance_matrix(&item->mesh, &status);
		compare_result = ecg::compare_mat3(cov_matrix, ecg::null_mat3);
		ASSERT_EQ(status, ecg::status_code::SUCCESS);
		timer.end();
	}
}

TEST(ecg_api, is_mesh_closed) {
	ecg::ecg_status status;
	custom_timer_t timer;
	bool result = false;
	ecg::ecg_mesh_t mesh;

	timer.start();
	result = ecg::is_mesh_closed(nullptr, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(result);

	timer.start();
	result = ecg::is_mesh_closed(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(result);

	timer.start();
	mesh.vertexes_size = 1;
	mesh.vertexes = (ecg::vec3_base*)(1);
	result = ecg::is_mesh_closed(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_TRUE(result);

	// Test on simple, but real models
	auto& mesh_inst = ecg_meshes::get_instance();
	ecg::ecg_mesh_t true_test = mesh_inst.loaded_meshes_by_name["is_closed_mesh-true.obj"]->mesh;
	ecg::ecg_mesh_t false_test = mesh_inst.loaded_meshes_by_name["is_closed_mesh-false.obj"]->mesh;
	ecg::ecg_mesh_t& surface_1_non_manifold = mesh_inst.loaded_meshes_by_name["surface_1.obj"]->mesh;
	ecg::ecg_mesh_t& surface_2_non_manifold = mesh_inst.loaded_meshes_by_name["surface_2.obj"]->mesh;

	timer.start();
	result = ecg::is_mesh_closed(&true_test, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_TRUE(result);

	timer.start();
	result = ecg::is_mesh_closed(&false_test, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&surface_1_non_manifold, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&surface_2_non_manifold, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);
}

TEST(ecg_api, is_mesh_manifold) {
	ecg::ecg_status status;
	custom_timer_t timer;
	bool result = false;
	ecg::ecg_mesh_t mesh;

	timer.start();
	result = ecg::is_mesh_manifold(nullptr, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_FALSE(result);

	timer.start();
	mesh.vertexes_size = 3;
	mesh.vertexes = (ecg::vec3_base*)(1);
	result = ecg::is_mesh_manifold(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_FALSE(result);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	mesh.vertexes_size = 1;
	mesh.vertexes = (ecg::vec3_base*)(1);
	result = ecg::is_mesh_manifold(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::NOT_TRIANGULATED_MESH);
	ASSERT_FALSE(result);

	auto& mesh_inst = ecg_meshes::get_instance();
	ecg::ecg_mesh_t& closed_mesh = mesh_inst.loaded_meshes_by_name["is_closed_mesh-true.obj"]->mesh;
	ecg::ecg_mesh_t& surface_1_non_manifold = mesh_inst.loaded_meshes_by_name["surface_1.obj"]->mesh;
	ecg::ecg_mesh_t& surface_2_non_manifold = mesh_inst.loaded_meshes_by_name["surface_2.obj"]->mesh;
	ecg::ecg_mesh_t& not_closed_mesh = mesh_inst.loaded_meshes_by_name["is_closed_mesh-false.obj"]->mesh;
	ecg::ecg_mesh_t& self_intersected_1 = mesh_inst.loaded_meshes_by_name["self_intersected_mesh_1.obj"]->mesh;
	ecg::ecg_mesh_t& self_intersected_2 = mesh_inst.loaded_meshes_by_name["self_intersected_mesh_2.obj"]->mesh;
	ecg::ecg_mesh_t& sandglass_non_manifold = mesh_inst.loaded_meshes_by_name["sandglass-non-manifold.obj"]->mesh;

	timer.start();
	result = ecg::is_mesh_manifold(&closed_mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_TRUE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&not_closed_mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&sandglass_non_manifold, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&surface_1_non_manifold, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&surface_2_non_manifold, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&self_intersected_1, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_manifold(&self_intersected_2, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);
}

TEST(ecg_api, is_mesh_self_intersected) {
	auto invalid_method = ecg::self_intersection_method::METHODS_COUNT;
	auto method = ecg::self_intersection_method::BRUTEFORCE;
	ecg::ecg_status status;
	custom_timer_t timer;
	ecg::ecg_mesh_t mesh;
	bool result = false;

	timer.start();
	result = ecg::is_mesh_self_intersected(nullptr, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_self_intersected(&mesh, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_FALSE(result);

	timer.start();
	mesh.vertexes_size = 1;
	mesh.vertexes = (ecg::vec3_base*)(1);
	result = ecg::is_mesh_self_intersected(&mesh, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_FALSE(result);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	result = ecg::is_mesh_self_intersected(&mesh, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::NOT_TRIANGULATED_MESH);
	ASSERT_FALSE(result);

	timer.start();
	mesh.indexes_size = 3;
	result = ecg::is_mesh_self_intersected(&mesh, invalid_method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INCORRECT_METHOD);
	ASSERT_FALSE(result);

	auto& mesh_inst = ecg_meshes::get_instance();
	ecg::ecg_mesh_t& surface_1_non_manifold = mesh_inst.loaded_meshes_by_name["surface_1.obj"]->mesh;
	ecg::ecg_mesh_t& surface_2_non_manifold = mesh_inst.loaded_meshes_by_name["surface_2.obj"]->mesh;
	ecg::ecg_mesh_t& not_self_intersected = mesh_inst.loaded_meshes_by_name["is_closed_mesh-true.obj"]->mesh;
	ecg::ecg_mesh_t& self_intersected_1 = mesh_inst.loaded_meshes_by_name["self_intersected_mesh_1.obj"]->mesh;
	ecg::ecg_mesh_t& self_intersected_2 = mesh_inst.loaded_meshes_by_name["self_intersected_mesh_2.obj"]->mesh;

	timer.start();
	result = ecg::is_mesh_self_intersected(&surface_1_non_manifold, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_self_intersected(&surface_2_non_manifold, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_self_intersected(&not_self_intersected, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_FALSE(result);

	timer.start();
	result = ecg::is_mesh_self_intersected(&self_intersected_1, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_TRUE(result);

	timer.start();
	result = ecg::is_mesh_self_intersected(&self_intersected_2, method, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_TRUE(result);
}

TEST(ecg_api, triangulate_mesh) {
	ecg::ecg_status status;
	ecg::ecg_array_t res;
	custom_timer_t timer;
	ecg::ecg_mesh_t mesh;

	timer.start();
	res = ecg::triangulate_mesh(nullptr, 0, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(res.arr_ptr == nullptr);

	timer.start();
	res = ecg::triangulate_mesh(&mesh, 1, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_TRUE(res.arr_ptr == nullptr);

	timer.start();
	mesh.vertexes_size = 1;
	mesh.vertexes = (ecg::vec3_base*)(1);
	res = ecg::triangulate_mesh(&mesh, 1, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_TRUE(res.arr_ptr == nullptr);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	res = ecg::triangulate_mesh(&mesh, 3, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INCORRECT_VERTEX_COUNT_IN_FACE);
	ASSERT_TRUE(res.arr_ptr == nullptr);

	auto& mesh_inst = ecg_meshes::get_instance();
	ecg::ecg_mesh_t& not_triangulated_mesh_1 = mesh_inst.loaded_meshes_by_name["not_triangulated_mesh_1.obj"]->mesh;

	timer.start();
	res = ecg::triangulate_mesh(&not_triangulated_mesh_1, 4, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::SUCCESS);
	ASSERT_TRUE((res.arr_size / sizeof(uint32_t)) % 3 == 0);
	ASSERT_TRUE(res.arr_ptr != nullptr);
	ASSERT_TRUE(res.arr_size > 0);
}

TEST(ecg_api, compute_volume) {
	constexpr float epsilon = std::numeric_limits<float>::epsilon();
	ecg::ecg_status status;
	custom_timer_t timer;
	ecg::ecg_mesh_t mesh;
	float res;

	timer.start();
	res = ecg::compute_volume(nullptr, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::INVALID_ARG);
	ASSERT_TRUE(res == -1.0f);

	timer.start();
	res = ecg::compute_volume(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_VERTEX_ARR);
	ASSERT_TRUE(res == -1.0f);

	timer.start();
	mesh.vertexes_size = 1;
	mesh.vertexes = (ecg::vec3_base*)(1);
	res = ecg::compute_volume(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::EMPTY_INDEX_ARR);
	ASSERT_TRUE(res == -1.0f);

	timer.start();
	mesh.indexes_size = 4;
	mesh.indexes = (uint32_t*)(1);
	res = ecg::compute_volume(&mesh, &status);
	timer.end();

	ASSERT_EQ(status, ecg::status_code::NOT_TRIANGULATED_MESH);
	ASSERT_TRUE(res == -1.0f);

	auto& mesh_inst = ecg_meshes::get_instance();
	ecg::ecg_mesh_t& default_cube = mesh_inst.loaded_meshes_by_name["default_cube.obj"]->mesh;

	timer.start();
	res = ecg::compute_volume(&default_cube, &status);
	timer.end();

	ASSERT_TRUE((res - 8.0f) <= epsilon);
}