#include <help/ecg_allocate.h>

namespace ecg {
	ecg_cl_mesh_t allocate_cl_mesh(const ecg_mesh_t* mesh) {
		auto& ctrl = ecg_cl::get_instance();
		auto queue = ctrl.get_cmd_queue();
		auto context = ctrl.get_context();
		auto device = ctrl.get_device();

		ecg_status_handler op_res;
		ecg_cl_mesh_t cl_mesh;

		try {
			queue.finish();
			{
				cl_mesh.is_valid = false;

				cl_mesh.indexes_size = mesh->indexes_size;
				cl_mesh.vertexes_size = mesh->vertexes_size;
				cl_mesh.indexes_buffer_size = sizeof(uint32_t) * cl_mesh.indexes_size;
				cl_mesh.vertexes_buffer_size = sizeof(vec3_base) * cl_mesh.vertexes_size;

				int err_create_buffer = CL_SUCCESS;
				cl_mesh.vertexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, cl_mesh.vertexes_buffer_size, nullptr, &err_create_buffer);  op_res = err_create_buffer;
				cl_mesh.indexes_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, cl_mesh.indexes_buffer_size, nullptr, &err_create_buffer);  op_res = err_create_buffer;

				queue.enqueueFillBuffer(cl_mesh.vertexes_buffer, CL_FALSE, 0, cl_mesh.vertexes_buffer_size);
				queue.enqueueFillBuffer(cl_mesh.indexes_buffer, CL_FALSE, 0, cl_mesh.indexes_buffer_size);
			}
			queue.finish();

			cl_mesh.is_valid = true;
		}
		catch (...) {
			cl_mesh = ecg_cl_mesh_t();
		}

		return cl_mesh;
	}
}