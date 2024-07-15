#include <ecg_api.h>

namespace ecg {
#if defined(_DEBUG) && defined(SYCL_BUILD)
	ecg_status ecg_debug_func() {
		const std::string secret{
			"Ifmmp-!xpsme\"\012J(n!tpssz-!Ebwf/!"
			"J(n!bgsbje!J!dbo(u!ep!uibu/!.!IBM\01"
		};
		const auto sz = secret.size();

		sycl::queue q;
		char* result = sycl::malloc_shared<char>(sz, q);
		std::memcpy(result, secret.data(), sz);

		q.parallel_for(sz, [=](auto& i) {
			result[i] -= 1;
		}).wait();

		std::cout << result << "\n";
		sycl::free(result, q);
		return SUCCESS;
	}
#elif defined(OPENCL_BUILD)
	ecg_status ecg_debug_func() {
		return SUCCESS;
	}
#endif
}