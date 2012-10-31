/*
 *      Copyright (C) 2011 Garrett Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GPIO.h"

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define COUNT 100

void test_write(CGPIO &gpio)
{
	gpio.SetDirection(CGPIO::OUT);
	gpio.SetEdge(CGPIO::BOTH);
	gpio.SetValue(0);
}

void test_read(CGPIO &gpio, double &avg, double &stddev)
{
	gpio.SetDirection(CGPIO::IN);
	gpio.GetValue();

	long data[COUNT];
	struct timeval start, stop;
	struct timespec delay;

	avg = 0;
	stddev = 0;

	gpio.SetDirection(CGPIO::IN);
	gpio.GetValue();

	for (int i = 0; i < COUNT; i++)
	{
		gettimeofday(&start, NULL);
		gpio.GetValue();
		gettimeofday(&stop, NULL);
		data[i] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;

		// Sleep for 10337ns (prime number so we don't align to any system timers)
		delay.tv_sec = 0;
		delay.tv_nsec = 10337;
		nanosleep(&delay, NULL);
	}

	// Compute the mean
	for (int i = 0; i < COUNT; i++)
		avg += data[i];
	avg /= COUNT;

	// Compute the standard deviation
	for (int i = 0; i < COUNT; i++)
		stddev += (data[i] - avg) * (data[i] - avg);
	stddev = sqrt(stddev / (COUNT - 1));

}

/*
void test_pwm(CGPIO &gpioCGPIO &gpio)
{
	printf("Pulsing pin 139 with 1s highs and 3s lows for 10s\n");
	gpio139.PWM(4 * 1000 * 1000, 10 * 1000 * 1000, 0.25);

	printf("Pulsing pin 139 with 3s highs and 1s lows for 10s\n");
	gpio139.PWM(4 * 1000 * 1000, 10 * 1000 * 1000, 0.75);

	printf("Hello World!\n");
}
*/

void test_poll(CGPIO &gpio)
{

}

void test_mirror(CGPIO &gpio)
{

}


int main (int argc, char** argv)
{
	const int gpio_pin = 136;
	double avg;
	double stddev;

	CGPIO gpio(gpio_pin);
	try
	{
		gpio.Open();

		const char ESC = 27;
		printf("Welcome to the %c[1mAperture Science Testing Initiative%c[0m.\n\n", ESC, ESC);

		//test_write(gpio);

		test_read(gpio, avg, stddev);
		printf("Average: %f\nStandard deviation: %f\n", avg, stddev);

		//test_pwm(gpio);

		//test_poll();

		//test_mirror();
	}
	catch (CGPIO::PermissionException &e)
	{
		printf("Could not export the GPIO pin. Try running as root\n");
	}
	catch (CGPIO::Exception &e)
	{
		printf("GPIO Exception occurred: %s\n", e.what());
	}
}

/*
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
using namespace std;

int main(int argc, char **argv)
{
	printf(
		"         UID           GID  \n"
		"Real      %d  Real      %d  \n"
		"Effective %d  Effective %d  \n",
		getuid (),     getgid (),
		geteuid(),     getegid()
	);

	// Need geteuid() to be 0 (root user)
	// To remedy:
	// sudo chown root Calibrate
	// sudo chmod ug+s Calibrate
	// sudo chmod o-rx Calibrate

	return 0;
}
*/

