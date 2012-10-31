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

#pragma once

#include <exception>  // For std::exception
#include <sys/time.h> // For gettimeofday()

class GPIO
{
public:
	/**
	 * GPIO::Exception
	 *
	 * Thrown when a problem has occured in reading or writing to the sysfs.
	 *
	 * The post-condition of GPIO public functions is that the GPIO pin is in
	 * an open and usable state. If this cannot be guaranteed by the function,
	 * this exception is thrown before the function runs to completion.
	 */
	class Exception : public std::exception
	{
	public:
		Exception(const GPIO &gpio, const char *function, const char *msg) throw();
		virtual ~Exception() throw() { };
		virtual const char* what() const throw() { return m_msg; }
	private:
		char m_msg[128];
	};

	// Direction of the GPIO pin
	enum Direction
	{
		OUT,
		IN
	};

	// Event that generates an interrupt on the GPIO pin
	enum Edge
	{
		NONE    = 0,
		RISING  = 1,
		FALLING = 2,
		BOTH    = RISING | FALLING
	};

	/**
	 * A GPIO object is constructed in an invalid state and a call to Open()
	 * must be performed before other functions are called.
	 */
	GPIO(unsigned int gpio);

	/**
	 * The GPIO class follows a quasi-RAII model. A call to Open() acquires
	 * the resource (pin), and the resource is released when Close() is called
	 * or the object falls out of scope.
	 */
	~GPIO() throw() { /*Close();*/ }

	/**
	 * Open the pin by exporting it, recording its current state, and getting
	 * it ready to be manipulated.
	 */
	bool Open();

	/**
	 * Returns true if the pin is exported and ready to be manhandled.
	 */
	bool IsOpen() const { return m_gpio_fd >= 0; }

	/**
	 * Release the pin when we are done with it for use in another
	 * object/thread/process.
	 */
	void Close() throw();

	/*!
	 * Get the pin number of the GPIO pin.
	 *
	 * \return The pin number.
	 */
	unsigned int Describe() const { return m_gpio; }

	/*!
	 * Gets the value of the pin. For pin 139, the value would be read from
	 * /sys/class/gpio/gpio139/value.
	 *
	 * For performance reasons, the file descriptor is kept open. Tests on my
	 * BeagleBoard indicate that opening /value can take several hundred
	 * microseconds. If the fd is kept open, seeking to the beginning of /value
	 * takes 0 to tens of microseconds. Thus, the fd is only closed when
	 * necessary (i.e. changing direction so that /value may be re-opened as
	 * write-only). Additionally, seeking is performed immediately after every
	 * read and write to decrease the latency of these operations as much as
	 * possible.
	 *
	 * \throw GPIO::Exception
	 * \return The value of the pin (0 or 1)
	 *   * If direction is "in", this returns 0 or 1 corresponding to the pin
	 *     being connected to ground or VCC.
	 *   * If the direction is "out", this returns the value controlling the pin.
	 *   * If an error occurs (such as the pin not being exported or an
	 *     unknown value being read, then this throws the error.
	 */
	unsigned int GetValue();

	/*!
	 * Set the value controlling the GPIO pin. If the pin's direction is
	 * "in", this has no effect.
	 *
	 * \param value Either 0 or 1
	 * \throw GPIO::Exception
	 */
	void SetValue(unsigned int value);

	/*!
	 * Get the direction of the GPIO pin. IN means that values are ready
	 * to be read using GetValue(). OUT means that the pin is ready to be
	 * controlled using SetValue().
	 *
	 * \return The direction of type enum Direction (either OUT or IN).
	 */
	Direction GetDirection() const { return m_dir; }

	/*!
	 * Set the direction of the pin. The direction is controlled by writing
	 * one of four strings to /sys/class/gpio/gpioXXX/direction: "in",
	 * "out", "high" and "low". Out and low are equivalent. High will set
	 * the pin to "out" and /value will be set to 1.
	 *
	 * \param dir           The direction, either IN or OUT.
	 * \param initial_value The initial value (ignored if dir is IN).
	 * \throw GPIO::Exception
	 */
	void SetDirection(Direction dir, unsigned int initial_value = 0);

	/*!
	 * Get the signal edge(s) of the GPIO pin that will make Poll() return. As
	 * the trivial case, none will cause Poll() to return immediately. Rising
	 * will return when the pin transitions from a 0 to a 1; falling from a 1
	 * to a 0. An interesting case is "both": the OMAP/DM3730 processor
	 * internally only supports "change from last read", so when an interrupt
	 * occurs a dummy read is performed internally to reset this value.
	 *
	 * \throw GPIO::Exception
	 * \return The edge of type enum GPIO::Edge (NONE, RISING, FALLING, or BOTH).
	 */
	Edge GetEdge() const { return m_edge; }
	void SetEdge(Edge edge);

	/*!
	 * Wait for an interrupt to trigger the pin, and return when this happens.
	 * The type of interrupt is configured by SetEdge(). If the edge is set to
	 * NONE or the direction is set to OUT, this function returns immediately.
	 * Otherwise, it blocks until an interrupt is triggered.
	 *
	 * If Poll() times out the appropriate exception is thrown.
	 *
	 * \param  timeout The maximum time to wait (in microseconds).
	 * \param duration The duration of time until the interrupt occurred (microseconds).
	 *                 Guaranteed to not be greater than timeout.
	 * \param  verify  Guard against spurious wakeups by verifying the value after
	 *                 the interrupt occurs. For best performance, explicitly set
	 *                 this to false.
	 * \param  value   (Out) Only used if verify is true. If this is specified,
	 *                 the final value of the pin after a successful interrupt
	 *                 occurs will be placed here.
	 * \throw          GPIO::Exception
	 * \return false   if Poll() timed out or pin conditions are invalid
	 */
	bool Poll(unsigned long timeout, unsigned long &duration, bool verify = false,
			unsigned int *value = (unsigned int*)0);

	/*!
	 * Copy a value from one pin to another. This function can be chained:
	 *     gpio3.Mirror(gpio2.Mirror(gpio1));
	 *
	 * \param  pin_object The pin to read the value from.
	 * \throw  GPIO::Exception
	 */
	GPIO& Mirror(GPIO &pin_object);

	/*!
	 * Maintains a steady pulse on the GPIO pin. If the pin's direction is IN,
	 * this function has no effect and returns immediately. Otherwise, this
	 * function blocks until the falling edge of the final pulse. The time
	 * taken by Pulse() is equal to (2*duration*count), minus the duration of
	 * the final low. As a post-condition, the value is set to 0.
	 *
	 * \param  duration The duration of the pulse (in microseconds). If count > 1
	 *                  then this duration will also be used between each pulse.
	 * \param  count    The total number of pulses to deliver.
	 * \throw           GPIO::Exception
	 * \return true     If the specified time (2*duration*count) has elapsed
	 */
	bool Pulse(unsigned long duration, unsigned int count = 1);

	/*!
	 * Similar to Pulse(), this function allows you to set the duty cycle of
	 * the pulse. If duty_cycle is 0.5 then this function behaves like Pulse()
	 * with the duration equal to half the period. As a post-condition, the
	 * value of the GPIO pin is set to 0.
	 *
	 * \param period     The period of the pulse (time between each rising edge) in
	 *                   microseconds.
	 * \param time       The length of the PWM signal in microseconds. PWM() blocks
	 *                   at least for this time until the final pulse falls.
	 * \param duty_cycle The PWM duty cycle between 0 and 1. A duty cycle of 0 or 1
	 *                   will force the signal fully on or fully off, respectively.
	 * \throw           GPIO::Exception
	 * \return true     If the specified time has elapsed, similar to Pulse()
	 */
	bool PWM(unsigned long period, unsigned long time, double duty_cycle = 0.5);

private:
	/*!
	 * Exporting a pin means to enable its use for GPIO functionality. Pins are
	 * exported by writing their number to /sys/class/gpio/export, for example:
	 * echo 139 > /sys/class/gpio/export. The export sysfs file must be write-
	 * enabled for the current user.
	 */
	bool Export();

	/*!
	 * Disable the GPIO pin. This is done by writing the pin number to
	 * /sys/class/gpio/unexport, similar to Open(). The GPIO sysfs directory
	 * is deleted, so make sure that all file connections (value, direction and
	 * edge) are closed before unexporting the pin.
	 */
	void Unexport() throw(); // Called by destructor, no exceptions allowed

	/*!
	 * Reopen the pin's value node in the specified RW mode. No function exists
	 * for getting the current RW mode because it is assumed that a direction
	 * of IN means read-only and a direction of OUT means write-only. The GPIO
	 * class maintains this consistency.
	 *
	 * \param mode One of: O_RDONLY (00), O_WRONLY (01)
	 * \throw GPIO::Exception
	 */
	void Reopen(int mode);

	/*!
	 * Sync m_dir with /sys/class/gpio/gpioXXX/direction. This allows m_dir to
	 * be used as a caching variable so we don't have to hit the sysfs every
	 * time we want to know the direction.
	 *
	 * \throw GPIO::Exception
	 */
	void ReadDirection();

	/*!
	 * Sync m_edge with /sys/class/gpio/gpioXXX/edge. This allows m_edge to be
	 * used as a caching variable so we don't have to hit the sysfs every time
	 * we want to know the edge.
	 *
	 * \throw GPIO::Exception if, for whatever reason, m_edge is not set,
	 */
	void ReadEdge();

	/*!
	 * Calculate the number of microseconds elapsed since the reference time.
	 *
	 * \param start The reference time. If start is in the future, the return value
	 *              will be negative.
	 * \return      The time delta.
	 */
	static inline long Elapsed(struct timeval &start)
	{
		struct timeval end;
		gettimeofday(&end, 0);
		return (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	}

private:
	/**
	 * This object is noncopyable.
	 */
	GPIO(const GPIO &other);
	GPIO& operator=(const GPIO &rhs);

	// GPIO pin number for this class
	unsigned int m_gpio;
	// String representation (used internally)
	char m_str_gpio[5];
	// File descriptor to the GPIO pin
	int m_gpio_fd;
	// Current direction of the GPIO pin (IN or OUT)
	Direction m_dir;
	// Edge configured to generate interrupts (only used if m_dir is IN)
	Edge m_edge;
};
