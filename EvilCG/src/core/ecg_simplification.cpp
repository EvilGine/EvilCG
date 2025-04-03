#include <core/ecg_simplification.h>

#include <core/ecg_subprograms.h>
#include <core/ecg_host_ctrl.h>
#include <core/ecg_internal.h>
#include <core/ecg_program.h>

namespace ecg {
	void center_point_simplification(const ecg_mesh_t* mesh, ecg_internal_mesh& result_mesh, ecg_status_handler& op_res) {
		auto& ctrl = ecg_host_ctrl::get_instance();
		auto& queue = ctrl.get_cmd_queue();
		auto& context = ctrl.get_context();
		auto& dev = ctrl.get_device();

		cl::Program::Sources sources = { center_point_simplification_code };
		ecg_program program(context, dev, sources);

		cl_uint indexes_size = mesh->indexes_size;
		cl_uint vertexes_size = mesh->vertexes_size;
		cl_int vrt_size = sizeof(vec3_base) / sizeof(float);
		size_t indexes_buffer_size = sizeof(uint32_t) * indexes_size;
		size_t vertexes_buffer_size = sizeof(vec3_base) * vertexes_size;

		cl_int err_create_buffer = CL_SUCCESS;
		cl::Buffer vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, vertexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
		cl::Buffer indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, indexes_buffer_size, nullptr, &err_create_buffer); op_res = err_create_buffer;
		
		cl_uint result_indexes_size = indexes_size / 3;
		cl_uint result_vertexes_size = vertexes_size / 3;
		cl::Buffer result_vertexes_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, vertexes_buffer_size / 3, &err_create_buffer); op_res = err_create_buffer;
		cl::Buffer result_indexes_size_buffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(uint32_t), &err_create_buffer); op_res = err_create_buffer;

		cl::NDRange global = mesh->vertexes_size;
		cl::NDRange local = cl::NullRange;

		op_res = queue.enqueueWriteBuffer(vertexes_buffer, CL_FALSE, 0, vertexes_buffer_size, mesh->vertexes);
		op_res = queue.enqueueWriteBuffer(indexes_buffer, CL_FALSE, 0, indexes_buffer_size, mesh->indexes);
		op_res = queue.finish();

		op_res = program.execute(
			queue, center_point_simplification_name, global, local,
			vertexes_buffer, vertexes_size, indexes_buffer, indexes_size,
			// Result mesh
			result_vertexes_buffer, result_vertexes_size,
			result_indexes_size_buffer, vrt_size
		);

		//op_res = queue.enqueueReadBuffer();
		op_res = queue.finish();
	}

	void qem_simplification(const ecg_mesh_t* mesh, ecg_internal_mesh& result_mesh, ecg_status_handler& op_res) {
		throw std::logic_error("Not implemented method"); // TODO: implement later this method
	}
}