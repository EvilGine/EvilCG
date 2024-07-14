#include <ecg_api.h>

namespace ecg {
    namespace compute {
        ecg_status calculate_normals(mesh_t* mesh) {
            ecg_status status = check_mesh(mesh);
            if (status != SUCCESS) return status;
            
            sycl::queue main_queue(sycl::default_selector_v);

            return SUCCESS;
        }
    }
}