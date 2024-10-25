#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ecg_meshes.h>

int main(int argc, char** argv)
{
	auto& meshes_inst = ecg_meshes::get_instance();
	meshes_inst.load_meshes("./Models");

	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);

	return RUN_ALL_TESTS();
}