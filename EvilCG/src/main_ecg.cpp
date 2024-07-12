#include <cmath>
#include <iostream>
#include <CL/sycl.hpp>
#include <glm/glm.hpp>
#include <ecg_api.h>

int test_function()
{
    sycl::queue queue;
    glm::vec3 test;

    std::cout << "[STAGE]:> " << 1 << std::endl;
    std::cout << "Using "
        << queue.get_device().get_info<sycl::info::device::name>()
        << std::endl;

    std::cout << "[STAGE]:> " << 2 << std::endl;
    // Compute the first n_items values in a well known sequence
    constexpr int n_items = 16;
    int* items = sycl::malloc_shared<int>(n_items, queue);
    queue.parallel_for(sycl::range<1>(n_items), [items](sycl::id<1> i) {
        double x1 = pow((1.0 + sqrt(5.0)) / 2, i);
        double x2 = pow((1.0 - sqrt(5.0)) / 2, i);
        items[i] = round((x1 - x2) / sqrt(5));
        }).wait();

    std::cout << "[STAGE]:> " << 3 << std::endl;
    for (int i = 0; i < n_items; ++i) {
        std::cout << items[i] << std::endl;
    }

    std::cout << "[STAGE]:> " << 4 << std::endl;
    free(items, queue);
    return 0;
}