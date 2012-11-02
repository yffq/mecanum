//#include "AVRController.h"
//#include "ParamServer.h"
#include "BBExpansionPin.h"
#include "Thumbwheel.h"

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

bool bTestButtons = true;

void TestButton(const char *color, unsigned int expansionPin)
{
	BBExpansionPin gpio(expansionPin);
	ASSERT_TRUE(gpio.Open());
	EXPECT_NO_THROW(gpio.SetDirection(GPIO::IN));
	EXPECT_NO_THROW(gpio.SetEdge(GPIO::BOTH));
	unsigned int value;
	EXPECT_NO_THROW(value = gpio.GetValue());
	EXPECT_EQ(value, 1);

	cout << "Press the " << color << " button (" << (BUTTON_TIMEOUT / 1000000) << "s)" << endl;
	unsigned long duration = 0;
	unsigned int post_value = 2;
	EXPECT_TRUE(gpio.Poll(BUTTON_TIMEOUT, duration, true, post_value));
	EXPECT_TRUE(duration > 0);
	EXPECT_EQ(post_value, 0);
}

TEST(GPIOTest, green)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("green", 3)); // 139
}

TEST(GPIOTest, yellow)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("yellow", 5)); // 138
}

TEST(GPIOTest, red)
{
	if (bTestButtons)
		EXPECT_NO_THROW(TestButton("red", 8)); // 143
}

void TestThumbwheel(Thumbwheel &tw, unsigned int target)
{
	if (tw.GetValue() != target)
	{
		cout << "Move thumbwheel to " << target << " (" << (BUTTON_TIMEOUT / 1000000) << "s)" << endl;
		unsigned int value;
		EXPECT_TRUE(tw.Poll(BUTTON_TIMEOUT, value));
		EXPECT_EQ(value, target);
	}
}

TEST(GPIOTest, thumnbwheel)
{
	if (bTestButtons)
	{
		Thumbwheel tw;
		ASSERT_TRUE(tw.Open());
		EXPECT_NO_THROW(TestThumbwheel(tw, 0));
		EXPECT_NO_THROW(TestThumbwheel(tw, 1));
		EXPECT_NO_THROW(TestThumbwheel(tw, 2));
		EXPECT_NO_THROW(TestThumbwheel(tw, 4));
	}
}

int main(int argc, char **argv)
{
	cout << "Test hardware buttons? (y/n)" << endl;
	bTestButtons = (cin.get() == 'y');

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
