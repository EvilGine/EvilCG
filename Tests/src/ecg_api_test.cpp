#ifdef _DEBUG
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define ENABLE_ECG_CL
#include <ecg_api.h>

TEST(ecg_api, init_ecg) {
	ecg::ecg_host_ctrl& host_ctrl = ecg::ecg_host_ctrl::get_instance();
	auto device = host_ctrl.get_main_device();
	auto queue = host_ctrl.get_cmd_queue();
	auto context = host_ctrl.get_context();

	ASSERT_NE(device, cl::Device());
	ASSERT_NE(context, cl::Context());
	ASSERT_NE(queue, cl::CommandQueue());
}
#endif