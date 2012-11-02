/*
 *  	  Copyright (C) 2011 Garrett Brown
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

#include <errno.h>    // for errno
#include <fcntl.h>    // for open()
#include <iostream>   // for cerr
#include <poll.h>     // for poll()
#include <stdio.h>    // for snprintf()
#include <stdlib.h>   // for system()
#include <string.h>   // for strerror()
#include <sys/stat.h> // for stat()
#include <sys/time.h> // for gettimeofday()
#include <time.h>     // for nanosleep()
#include <unistd.h>   // for I/O functions

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define EXPORT_COMMAND "`rospack find avr_controller`/gpio_export.sh %d"

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET -1
#endif

GPIO::Exception::Exception(const GPIO &gpio, const char *function, const char *msg) throw()
	: m_pin(gpio.Describe())
{
	snprintf(m_msg, sizeof(m_msg), "%s - Pin %d: %s", function, m_pin, msg);
}

GPIO::GPIO(unsigned int gpio) : m_gpio(gpio), m_gpio_fd(INVALID_SOCKET), m_dir(IN), m_edge(NONE)
{
	snprintf(m_str_gpio, sizeof(m_str_gpio), "%d", m_gpio);
}

bool GPIO::Open()
{
	// If the pin is already open, don't do anything
	if (!IsOpen())
	{
		// First start by checking to see if the pin is exported
		struct stat st;
		char gpio_dir[sizeof(SYSFS_GPIO_DIR "/gpio%d") + 5]; // 5 digits for GPIO number
		snprintf(gpio_dir, sizeof(gpio_dir), SYSFS_GPIO_DIR "/gpio%d", m_gpio);
		if (stat(gpio_dir, &st) != 0)
		{
			Export();
			if (stat(gpio_dir, &st) != 0)
			{
				std::cerr << "GPIO::Open - Pin " << m_gpio << ": Unable to export pin" << std::endl;
				return false;
			}
		}

		// Now, sync m_dir and m_edge with the sysfs values
		try
		{
			ReadDirection();
			ReadEdge();
			Reopen(m_dir == OUT ? O_WRONLY : O_RDONLY);
		}
		catch (const Exception &e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}
	}
	return true;
}

void GPIO::Close() throw()
{
	if (IsOpen())
	{
		close(m_gpio_fd);
		m_gpio_fd = INVALID_SOCKET;
	}
}

void GPIO::Export()
{
	char cmd_buffer[sizeof(EXPORT_COMMAND) + 5]; // allocate 5 digits
	snprintf(cmd_buffer, sizeof(cmd_buffer), EXPORT_COMMAND, m_gpio);
	int ret = system(cmd_buffer);
	(void)ret;
}

/**
 * Post-condition: Pin is ready for reading/writing. An exception is thrown to
 * indicate an invalid pin.
 */
void GPIO::Reopen(int mode)
{
	if (IsOpen())
	{
		close(m_gpio_fd);
		m_gpio_fd = INVALID_SOCKET;
	}
	char value_buffer[sizeof(SYSFS_GPIO_DIR "/gpio%d/value") + 5];
	snprintf(value_buffer, sizeof(value_buffer), SYSFS_GPIO_DIR "/gpio%d/value", m_gpio);
	m_gpio_fd = open(value_buffer, mode);
	if (!IsOpen())
		throw Exception(*this, __func__, strerror(errno));
}

/**
 * Post-condition: If m_dir is IN, the pin is left in a state such that the
 * next read encounters as little latency as possible. If m_dir is OUT, the
 * pin is left in a state such that the next call to SetValue() hits minimal
 * latency.
 *
 * If an exception is thrown, the pin will be in one of three states:
 *    1. Invalid (unexported)
 *    2. Inconsistent -- Close() must be called
 *    3. Ready for another read; the exception was just ephemeral
 */
unsigned int GPIO::GetValue()
{
	char ch; // '0' or '1'
	ssize_t num; // number of bytes read
	int prev_mode = O_RDONLY;

	// If direction is out, assume /value is write-only and re-open in read-only mode
	if (m_dir == OUT)
	{
		prev_mode = O_WRONLY;
		Reopen(O_RDONLY);
	}

	// Read the value from /sys/class/gpio/gpioXXX/value
	num = read(m_gpio_fd, &ch, 1);

	// If the read failed, close the file so we can try again
	if (num < 0)
	{
		int read_errno = errno;
		Reopen(prev_mode);
		throw Exception(*this, "GPIO::GetValue", strerror(read_errno));
	}

	if (num != 1)
	{
		Reopen(prev_mode);
		throw Exception(*this, "GPIO::GetValue", "Failed to read value");
	}

	// Re-open the file as write-only so SetValue() can do its thing
	if (prev_mode == O_WRONLY)
		Reopen(O_WRONLY);
	else
	{
		// Seek to the beginning so the next read encounters less latency
		if (lseek(m_gpio_fd, 0, SEEK_SET) < 0)
			throw Exception(*this, "GPIO::GetValue", strerror(errno));
	}

	// If we read a character, return 1 for everything not '0'
	return ch != '0' ? 1 : 0;
}

/**
 * Post-condition: pin is placed in a state such that the next call to
 * SetValue() encounters as little latency as possible.
 *
 * If an exception is thrown, the value may or may not have been written to the
 * pin, and the object may or may not be in an inconsistent state.
 */
void GPIO::SetValue(unsigned int value)
{
	// No effect if direction is IN, also prevents writing to a read-only file
	if (m_dir == IN)
		return;

	int num;
	num = write(m_gpio_fd, value ? "1" : "0", 2);

	// If we encountered a problem, raise an exception to let the caller know
	if (num < 0)
		throw Exception(*this, "GPIO::SetValue", strerror(errno));
	else if (num != 2)
		throw Exception(*this, "GPIO::SetValue", "Failed to write value");
	else
	{
		// Seek to the beginning so the next write encounters less latency
		if (lseek(m_gpio_fd, 0, SEEK_SET) < 0)
			throw Exception(*this, "GPIO::SetValue", strerror(errno));
	}
}

void GPIO::ReadDirection()
{
	// Open /sys/class/gpio/gpioXXX/direction for reading
	int fd_dir;
	char buffer_dir[sizeof(SYSFS_GPIO_DIR "/gpio%d/direction") + 5];
	snprintf(buffer_dir, sizeof(buffer_dir), SYSFS_GPIO_DIR "/gpio%d/direction", m_gpio);
	fd_dir = open(buffer_dir, O_RDONLY);
	if (fd_dir < 0)
		throw Exception(*this, __func__, strerror(errno));

	ssize_t num; // number of bytes read
	char read_value[4];
	num = read(fd_dir, read_value, sizeof(read_value));
	close(fd_dir);

	if (num < 2) // minimum 2 chars for "in"
		throw Exception(*this, __func__, "Direction string is too short");

	if (strncmp(read_value, "in", 2) == 0)
		m_dir = IN;
	else if (strncmp(read_value, "out", 3) == 0 || // These three are equivalent.
			 strncmp(read_value, "low", 3) == 0 || // Only "out" should be read,
			 strncmp(read_value, "high", 4) == 0)  // but just in case.
		m_dir = OUT;
	else
		throw Exception(*this, __func__, "Unknown direction value");
}

void GPIO::SetDirection(Direction dir, unsigned int initial_value /* = 0 */)
{
	// If the pin is already set to this direction, there's no effect
	if (m_dir == dir)
		return;

	// Close the fd for the value node so it can be re-opened as read/write only
	if (IsOpen())
	{
		close(m_gpio_fd);
		m_gpio_fd = INVALID_SOCKET;
	}

	// Open /sys/class/gpio/gpioXXX/direction for writing
	int fd_dir;
	char buffer_dir[sizeof(SYSFS_GPIO_DIR "/gpio%d/direction") + 5];
	snprintf(buffer_dir, sizeof(buffer_dir), SYSFS_GPIO_DIR "/gpio%d/direction", m_gpio);
	fd_dir = open(buffer_dir, O_WRONLY);
	if (fd_dir < 0)
		throw Exception(*this, "GPIO::SetDirection", strerror(errno));

	bool success; // did the write succeed?

	// Write "in", "high" or "low" to /direction ("out" is the same as "low)
	if (dir == IN)
		success = (write(fd_dir, "in", 3) == 3);
	else
	{
		if (initial_value)
			success = (write(fd_dir, "high", 5) == 5);
		else
			success = (write(fd_dir, "low", 4) == 4);
	}
	close(fd_dir);

	if (!success)
		throw Exception(*this, "GPIO::SetDirection", "Error writing direction");

	// Record the new direction and re-open the value node in the correct mode
	m_dir = dir;
	Reopen(m_dir == OUT ? O_WRONLY : O_RDONLY);
}

void GPIO::ReadEdge()
{
	// Open /sys/class/gpio/gpioXXX/edge for reading
	int fd_edge;
	char buffer_edge[sizeof(SYSFS_GPIO_DIR "/gpio%d/edge") + 5];
	snprintf(buffer_edge, sizeof(buffer_edge), SYSFS_GPIO_DIR "/gpio%d/edge", m_gpio);
	fd_edge = open(buffer_edge, O_RDONLY);
	if (fd_edge < 0)
		throw Exception(*this, "GPIO::ReadEdge", strerror(errno));

	ssize_t num; // number of bytes read
	char read_value[7];
	num = read(fd_edge, read_value, sizeof(read_value)); // length of "falling"
	close(fd_edge);

	if (num < 4) // minimum 4 chars for "none"
		throw Exception(*this, "GPIO::ReadEdge", "Direction string is too short");

	// We just compare the first 4 bytes. Sometimes this is all that's read
	if (strncmp(read_value, "none", 4) == 0)
		m_edge = NONE;
	else if (strncmp(read_value, "rising", 4) == 0)
		m_edge = RISING;
	else if (strncmp(read_value, "falling", 4) == 0)
		m_edge = FALLING;
	else if (strncmp(read_value, "both", 4) == 0)
		m_edge = BOTH;
	else
		throw Exception(*this, "GPIO::ReadEdge", "Unknown edge value");
}

void GPIO::SetEdge(Edge edge)
{
	// If the edge for this pin is already set to this edge, there's no effect
	if (m_edge == edge)
		return;

	// Open /sys/class/gpio/gpioXXX/edge for writing
	int fd_edge;
	char buffer_edge[sizeof(SYSFS_GPIO_DIR "/gpio%d/edge") + 5];
	snprintf(buffer_edge, sizeof(buffer_edge), SYSFS_GPIO_DIR "/gpio%d/edge", m_gpio);
	fd_edge = open(buffer_edge, O_WRONLY);
	if (fd_edge < 0)
		throw Exception(*this, "GPIO::SetEdge", strerror(errno));

	bool success; // did the write succeed?

	switch (edge)
	{
	case NONE:
		success = (write(fd_edge, "none", 5) == 5);
		break;
	case RISING:
		success = (write(fd_edge, "rising", 7) == 7);
		break;
	case FALLING:
		success = (write(fd_edge, "falling", 8) == 8);
		break;
	case BOTH:
	default:
		success = (write(fd_edge, "both", 5) == 5);
		break;
	}
	close(fd_edge);
	if (!success)
		throw Exception(*this, "GPIO::SetEdge", "Error writing edge");

	// Record the new edge
	m_edge = edge;
}

bool GPIO::Poll(unsigned long timeout, unsigned long &duration, bool verify /* = true */)
{
	unsigned int value;
	bool ret = Poll(timeout, duration, verify, value);
	(void)value; // silence compiler warning
	return ret;
}

bool GPIO::Poll(unsigned long timeout, unsigned long &duration, bool verify, unsigned int &value)
{
	duration = 0;

	// Sanity checks
	if (m_dir == OUT)
	{
		std::cerr << "GPIO::Poll - GPIO pin " << m_str_gpio << " direction is set to OUT" << std::endl;
		return false;
	}
	if (m_edge == NONE)
	{
		std::cerr << "GPIO::Poll - GPIO pin " << m_str_gpio << " edge is set to NONE" << std::endl;
		return false;
	}
	if (timeout == 0)
	{
		return false;
	}

	int initial_value; // used to check for spurious wakeups
	int ret;           // poll() return value
	long remaining;
	struct pollfd fd_value_ptr[1];
	struct timeval start;

	gettimeofday(&start, NULL);

	// If m_edge == BOTH, this is used to verify the invariant
	if (verify && m_edge == BOTH)
		initial_value = GetValue();
	else
		initial_value = 2; // unused

	while ((remaining = timeout - Elapsed(start)) > 0)
	{
		memset(reinterpret_cast<void*>(fd_value_ptr), 0, sizeof(fd_value_ptr));
		fd_value_ptr[0].fd = m_gpio_fd;   // File descriptor to poll
		fd_value_ptr[0].events = POLLPRI; // Types of events we care about

		// Block until triggered by an interrupt or timeout occurs. Timeouts are
		// in milliseconds! To preserve the timeout condition that "duration
		// will be exactly equal to timeout", we conservatively round up (unless
		// remaining is an exact multiple of 1000) a.k.a. ceiling.
		ret = poll(fd_value_ptr, 1, (int)(remaining - 1) / 1000 + 1);

		if (ret > 0)
		{
			// ret is 1 (number of fd's changed); check return events
			if (fd_value_ptr[0].revents & POLLPRI)
			{
				// Get the elapsed time
				long time = Elapsed(start);
				bool false_alarm = false;

				// Kernel documentation states:
				//    "After poll(2) returns, either lseek(2) to the beginning
				//    of the sysfs file and read the new value or close the file
				//    and re-open it to read the value."
				// We lseek here to keep the fd tidy; GetValue() expects this. This lets
				// GetValue() offer the best performance next time we read.
				if (lseek(m_gpio_fd, 0, SEEK_SET) < 0)
					throw Exception(*this, "GPIO::Poll", strerror(errno));

				// Verify the invariant!
				if (verify)
				{
					int new_value = GetValue();
					// If we're unable to verify, do nothing
					if (new_value != -1)
					{
						switch (m_edge)
						{
						case RISING: // value should be 1
							if (new_value != 1)
								false_alarm = true;
							break;
						case FALLING: // value should be 0
							if (new_value != 0)
								false_alarm = true;
							break;
						case BOTH: // value should be !initial_value
							if (new_value == initial_value)
								false_alarm = true;
							break;
						case NONE:
						default:
							break; // Shouldn't be here
						}
						// Record the new value if it wasn't a false alarm
						if (!false_alarm)
							value = new_value;
					}
				}

				// If a false alarm was tripped, continue through and poll() again
				if (!false_alarm)
				{
					// Clamp time below timeout
					duration = (time <= (long)timeout) ? time : timeout;
					return true;
				}
				else
				{
					std::cerr << "GPIO::Poll - False alarm detected, probably harmless" << std::endl;
				}
				// Continue
			}
			else if (fd_value_ptr[0].revents & POLLERR)
			{
				// poll() encountered an error condition
				throw Exception(*this, "GPIO::Poll", "poll() was interrupted by an error");
			}
			else
			{
				// What else can cause poll() to return? Notify the user here
				char buffer[40];
				snprintf(buffer, sizeof(buffer), "Unknown poll(), \"revents\": %d\n", fd_value_ptr[0].revents);
				throw Exception(*this, "GPIO::Poll", buffer);
			}
			// Continue
		}
		else if (ret == 0)
		{
			// If ret is 0, the call timed out and no file descriptors were ready
			break;
		}
		else // ret < 0
		{
			// Negative value indicating error
			throw Exception(*this, "GPIO::Poll", strerror(errno));
		}
		// Continue
	}
	return false;

}

GPIO& GPIO::Mirror(GPIO &pin_object)
{
	if (this != &pin_object)
	{
		if (GetDirection() == IN)
			SetDirection(OUT, pin_object.GetValue());
		else
			SetValue(pin_object.GetValue());
	}
	return *this; // Enable chaining: gpio3.Mirror(gpio2.Mirror(gpio1));
}

/**
 * Post-condition: If m_dir is OUT, then the pin will be low when this function
 * exits. If both duration and low are not zero, then this function will block
 * for a time strictly-less-than duration * count (the final low doesn't block).
 *
 * Heads up, duration is specified in microseconds.
 */
bool GPIO::Pulse(unsigned long duration, unsigned int count /* = 1 */)
{
	// Can't pulse an input pin
	if (m_dir == IN)
		return false;

	// Ignore trivial cases (SetValue(0) is still called to fulfill the post-condition)
	if (duration != 0 && count != 0)
	{
		int sleeps = 3; // Number of awakening signals to tolerate
		struct timeval edge; // Edge timer
		struct timespec delay;
		long remaining;

		// Adjust count to be total number of flip flops (we don't block on final low)
		count = count * 2 - 1;

		// Start the counter as late as possible
		gettimeofday(&edge, NULL);
		do
		{
			// On odd count, raise the pin; on even count, lower the pin
			SetValue(count % 2);

			// Figure out when the next edge will occur
			int carry_seconds = (edge.tv_usec + duration) / 1000000;
			edge.tv_usec = (edge.tv_usec + duration) % 1000000;
			edge.tv_sec = edge.tv_sec + carry_seconds;

			// Elapsed() returns a negative value because edge is in the future
			while ((remaining = -Elapsed(edge)) > 0)
			{
				delay.tv_sec = (time_t) (remaining / 1000000);
				delay.tv_nsec = (remaining % 1000000) * 1000;

				// Note how the nanosleep() occurs as close to SetValue()
				// as possible (given that while loops wrap around)
				if (nanosleep(&delay, NULL) == -1 && !--sleeps)
				{
					// If a signal awakens nanosleep(), give up after 3 interruptions
					return false;
				}
			}
		} while (--count);
	}
	SetValue(0);
	return true;
}

/**
 * Post-condition: If m_dir is OUT, the pin's value is set to 0. If m_dir is IN
 * or time is 0, then this function exits immediately.
 *
 * Heads up, period and time are specified in microseconds.
 */
bool GPIO::PWM(unsigned long period, unsigned long time, double duty_cycle /* = 0.5 */)
{
	// Can't pulse an input pin
	if (m_dir == IN || time == 0)
		return false;

	long duration = (long)(period * duty_cycle); // microseconds
	long remaining; // microseconds
	int sleeps = 3; // give up after 3 interruptions
	struct timeval start, edge; // Edge timer
	struct timespec delay;

	// Simplify the trivial cases: 0% or 100% duty cycle, just set the
	// value and wait for "duration" microseconds
	if (duration <= 0 || duration >= (long)period)
	{
		struct timespec rem;
		SetValue(duty_cycle <= 0 ? 0 : 1);
		do
		{
			delay.tv_sec = (time_t)(time / 1000000);
			delay.tv_nsec = (time % 1000000) * 1000;
			// I chose not to throw an ADDException here
			if (nanosleep(&delay, &rem) == -1) // errno == EINTR
				time = rem.tv_sec * 1000000 + rem.tv_nsec / 1000;
			else
				time = 0;
		} while (time > 0 && --sleeps);

		// Fulfill the post-condition
		if (duty_cycle >= 1)
			SetValue(0);
	}
	else
	{
		gettimeofday(&start, NULL);
		edge.tv_sec = start.tv_sec;
		edge.tv_usec = start.tv_usec;

		while (true)
		{
			Pulse((unsigned long)duration);

			if (Elapsed(start) >= (long)time)
				break;

			// Figure out when the next edge will occur
			int carry_seconds = (edge.tv_usec + period) / 1000000;
			edge.tv_usec = (edge.tv_usec + period) % 1000000;
			edge.tv_sec = edge.tv_sec + carry_seconds;

			// Elapsed() returns a negative value because edge is in the future
			while ((remaining = -Elapsed(edge)) > 0)
			{
				delay.tv_sec = (time_t)(remaining / 1000000);
				delay.tv_nsec = (remaining % 1000000) * 1000;
				// If a signal awakens nanosleep(), give up after 3 interruptions
				if (nanosleep(&delay, NULL) == -1 && !--sleeps)
					return false;;
			}
		}
	}
	return true;
}
