#include "AVRController.h"
#include "ParamServer.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace std;

AVRController arduino;

TEST(AVRTest, open)
{
	ASSERT_TRUE(arduino.Open("/dev/ttyACM0")) << "Open succeed\n";
	ASSERT_TRUE(arduino.IsOpen()) << "IsOpen succeed\n";
}


int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
