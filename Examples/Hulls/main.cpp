#include "others.h"
#include <ecg_api.h>

int main() {
	std::cout << "[EvilCG] Hulls example\n";
	ecg::ecg_status status = ecg::ecg_status_code::SUCCESS;

	// 1. Load data
	const char* file = "rabbit.obj";
	auto internal_mesh = ecg::load_mesh(file, &status);
	ecg::ecg_mesh_t mesh = ecg::get_mesh_from_internal_mesh(internal_mesh);

	// 2. Calculate
	auto convex_hull = ecg::hulls::create_convex_hull(internal_mesh.vertexes, &status);
	std::cout << "[create_convex_hull] status = " << status << std::endl;

	auto aabb = ecg::hulls::compute_aabb(&mesh, &status);
	std::cout << "[compute_aabb] status = " << status << std::endl;

	auto obb = ecg::hulls::compute_obb(&mesh, &status);
	std::cout << "[compute_obb] status = " << status << std::endl;

	// 3. Save results
	auto convex_mesh = ecg::get_mesh_from_internal_mesh(convex_hull);
	ecg::save_mesh(&convex_mesh, "rabbit-convex-hull", ecg::ecg_file_type::ECG_OBJ_FILE, &status);
	save_bb_to_obj(&aabb, "rabbit-aabb.obj");
	save_bb_to_obj(&obb, "rabbit-obb.obj");

	// 4. Cleanup all allocated memory
	ecg::cleanup_all();
	return 0;
}