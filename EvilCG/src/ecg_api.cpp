#include <ecg_api.h>

#include <cl/ecg_subprograms.h>
#include <cl/ecg_host_ctrl.h>
#include <cl/ecg_program.h>

namespace ecg {
	vec3_base get_center(mesh_t* mesh, ecg_status* status) {
		return vec3_base();
	}

	vec3_base summ_vertexes(mesh_t* mesh, ecg_status* status) {
		return vec3_base();
	}

	bounding_box compute_aabb(mesh_t* mesh, ecg_status* status) {
		bounding_box result_bb = default_bb;
		ecg_status_handler op_res;

		try {
			auto& ctrl = ecg_host_ctrl::get_instance();
			auto& queue = ctrl.get_cmd_queue();
			auto& context = ctrl.get_context();
			auto& dev = ctrl.get_device();

			if (mesh == nullptr) { 
				op_res = status_code::INVALID_ARG;
				return result_bb;
			}

			if (mesh->vertexes == nullptr || mesh->vertexes_size <= 0) {
				op_res = status_code::EMPTY_VERTEX_ARR;
				return result_bb;
			}

			cl::Program::Sources sources = { compute_aabb_code };
			ecg_program program(context, dev, sources);

			const size_t buffer_size = sizeof(mesh->vertexes[0]) * mesh->vertexes_size;
			cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, buffer_size);
			cl::Buffer aabb_result = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(bounding_box));

			cl::NDRange local = cl::NullRange;
			cl::NDRange global = mesh->vertexes_size;
			cl_int vert_size = sizeof(mesh->vertexes[0]) / sizeof(float);

			op_res = queue.enqueueWriteBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &default_bb);
			op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, buffer_size, mesh->vertexes);
			op_res = program.execute(
				queue, compute_aabb_name, global, local,
				vertexes_buffer, vert_size,
				aabb_result);
			op_res = queue.enqueueReadBuffer(aabb_result, CL_FALSE, 0, sizeof(bounding_box), &result_bb);
		}
		catch (...) {
			if (op_res == status_code::SUCCESS)
				op_res = status_code::UNKNOWN_EXCEPTION;
			if (status != nullptr)
				*status = op_res.get_status();
		}

		return result_bb;
	}

	full_bounding_box compute_obb(mesh_t* mesh, ecg_status* status) {
		return full_bounding_box();
	}
}