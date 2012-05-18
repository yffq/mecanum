#ifndef FINITESTATEMACHINE_H
#define FINITESTATEMACHINE_H

/**
 *
 */
class FiniteStateMachine
{
public:
	virtual ~FiniteStateMachine() { }

	/**
	 *
	 */
	virtual void Step() = 0;

	/**
	 *
	 */
	virtual int Delay() const = 0;

	/**
	 *
	 *
	int Time()
	{
		unsigned long start = millis();
		this->Step();
		return static_cast<int>(millis() - start);
	}
	/**/
};

#endif // FINITESTATEMACHINE_H
