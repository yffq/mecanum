//#include "AVRController.h"
//#include "ParamServer.h"
#include "BBExpansionPin.h"

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
AVRController arduino;

TEST(AVRTest, test)
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
*/

#define BUTTON_TIMEOUT 10000000L  // 10.0s
#define THUMBWHEEL_SETTLE 1000000 // 1.0s

void TestButton(const char *color, unsigned int expansionPin)
{
	BBExpansionPin gpio(expansionPin);
	ASSERT_TRUE(gpio.Open());
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(gpio.SetEdge(GPIO::BOTH));
	unsigned int value;
	EXPECT_NO_THROW(value = gpio.GetValue());
	EXPECT_EQ(value, 1);

	cout << "Press the " << color << " button (10s)" << endl;
	unsigned long duration = 0;
	unsigned int post_value = 2;
	EXPECT_TRUE(gpio.Poll(BUTTON_TIMEOUT, duration, true, post_value));
	EXPECT_TRUE(duration > 0);
	EXPECT_EQ(post_value, 0);
}

TEST(GPIOTest, green)
{
	EXPECT_NO_THROW(TestButton("green", 3)); // 139
}

TEST(GPIOTest, yellow)
{
	EXPECT_NO_THROW(TestButton("yellow", 5)); // 138
}

TEST(GPIOTest, red)
{
	EXPECT_NO_THROW(TestButton("red", 8)); // 143
}

unsigned int GetThumbwheelValue(BBExpansionPin &tw1, BBExpansionPin &tw2, BBExpansionPin &tw4)
{
	return (1 - tw1.GetValue()) << 0 | (1 - tw2.GetValue()) << 1 | (1 - tw4.GetValue()) << 2;
}

void ThumbwheelTarget(BBExpansionPin &tw1, BBExpansionPin &tw2, BBExpansionPin &tw4, unsigned int target)
{
	unsigned long time = THUMBWHEEL_SETTLE;
	if (target == 0)
		time += 4 * 1000 * 1000; // Allow an extra 4 seconds to move to 0
	unsigned long duration = 0;
	cout << "Move thumbwheel to " << target << " (10s)" << endl;
	EXPECT_NO_THROW(tw1.Poll(BUTTON_TIMEOUT, duration, true));
	usleep(time);
	EXPECT_EQ(GetThumbwheelValue(tw1, tw2, tw4), target);
}

TEST(GPIOTest, thumnbwheel)
{
	BBExpansionPin tw1(19); // 131
	BBExpansionPin tw2(6); // 146
	BBExpansionPin tw4(7); // 137
	ASSERT_TRUE(tw1.Open());
	ASSERT_TRUE(tw2.Open());
	ASSERT_TRUE(tw4.Open());
	EXPECT_NO_THROW(tw1.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(tw2.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(tw4.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(tw1.SetEdge(GPIO::BOTH));
	EXPECT_NO_THROW(tw2.SetEdge(GPIO::BOTH));
	EXPECT_NO_THROW(tw4.SetEdge(GPIO::BOTH));

	unsigned int value;
	EXPECT_NO_THROW(value = GetThumbwheelValue(tw1, tw2, tw4));

	if (value > 0)
		ThumbwheelTarget(tw1, tw2, tw4, 0);

	ThumbwheelTarget(tw1, tw2, tw4, 1);
	ThumbwheelTarget(tw1, tw2, tw4, 2);
	ThumbwheelTarget(tw1, tw2, tw4, 3);
	ThumbwheelTarget(tw1, tw2, tw4, 4);
	ThumbwheelTarget(tw1, tw2, tw4, 5);
	ThumbwheelTarget(tw1, tw2, tw4, 6);
	ThumbwheelTarget(tw1, tw2, tw4, 7);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
