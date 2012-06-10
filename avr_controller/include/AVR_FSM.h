#ifndef AVR_FSM_H
#define AVR_FSM_H

/**
 * The representation of a FiniteStateMachine on the Arduino can be broken down
 * into two pieces: the parameters of how the state machine operates, and the
 * state itself. We use these parameters to uniquely identify an individual
 * FSM. Due to this, the constraint that a FSM not modify its own parameters
 * during operation is loosely enforced, as this would then constitute a
 * different FSM. The Arduino would then become out of sync with the
 * AVRController, necessitating communication if the Arduino decides to shuffle
 * its own FSMs.
 */
class AVR_FSM
{
public:
	AVR_FSM(unsigned char *properties, unsigned int length);

	/**
	 * Rule of three.
	 */
	AVR_FSM(const AVR_FSM &other);

	/**
	 * Rule of three.
	 */
	AVR_FSM &operator=(const AVR_FSM &src);

	/**
	 * Rule of three.
	 */
	~AVR_FSM() { delete[] m_properties; }

	/**
	 * Straight-forward comparison of two objects.
	 */
	bool operator==(const AVR_FSM &other) const;

	/**
	 * The difference between GetProperty() and the array index operator is
	 * that the latter allows the element to be modified.
	 */
	unsigned char &operator[](unsigned int i) const { return m_properties[i]; }

	/**
	 * Convenience function: the first parameter is the FSM's ID.
	 */
	unsigned char GetID() const { return m_properties[0]; }

	/**
	 * Returns the total number of parameters (including the initial ID).
	 */
	unsigned int GetLength() const { return m_length; }

private:
	unsigned char *m_properties;
	unsigned int m_length;
};

#endif // AVR_FSM_H
