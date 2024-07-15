#include <ecg_api.h>

namespace ecg {
    ecg_status check_mesh(mesh_t* mesh) {
        if (mesh == nullptr)
            return ARG_NULL_PTR;
        if (mesh->vertexes == nullptr || mesh->indexes == nullptr)
            return ARG_NULL_PTR;

        if (mesh->vertexes->size() == 0)
            return EMPTY_VERTEXES;
        if (mesh->indexes->size() == 0)
            return EMPTY_INDEXES;

        return SUCCESS;
    }

#if defined(SYCL_BUILD)
    ecg_status get_devices() {
        for (auto platform : sycl::platform::get_platforms()) {
            std::cout << "Platform: "
                    << platform.get_info<sycl::info::platform::name>()
                    << std::endl;

            for (auto device : platform.get_devices()) {
                std::cout << "Device: "
                        << device.get_info<sycl::info::device::name>()
                        << std::endl;
            }
        }

        return SUCCESS;
    }
#elif defined(OPENCL_BUILD)
    ecg_status get_devices() {
        return SUCCESS;
    }
#endif
}