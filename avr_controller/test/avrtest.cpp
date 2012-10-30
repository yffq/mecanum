#include "AVRController.h"
#include "ParamServer.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

AVRController arduino;

TEST(AVRTest, open)
{
	ASSERT_TRUE(arduino.Open("/dev/ttyACM0"));
	ASSERT_TRUE(arduino.IsOpen());

	vector<string> fsmv;
	arduino.ListFiniteStateMachines(fsmv);
	EXPECT_GT(fsmv.size(), 0);

	for (vector<string>::const_iterator it = fsmv.begin(); it != fsmv.end(); it++)
		arduino.DestroyFiniteStateMachine(*it);

	arduino.ListFiniteStateMachines(fsmv);
	EXPECT_EQ(fsmv.size(), 0);

	string xmastree;
	xmastree.push_back((uint8_t)FSM_CHRISTMASTREE);
	arduino.CreateFiniteStateMachine(xmastree);

	arduino.ListFiniteStateMachines(fsmv);
	EXPECT_EQ(fsmv.size(), 1);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
