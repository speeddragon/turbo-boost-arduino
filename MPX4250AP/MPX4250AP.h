/*
	MPX4250AP.h
*/
#ifndef MPX4250AP_h
#define MPX4250AP_h

#include <Arduino.h>

class MPX4250AP
{
	public: 
		MPX4250AP(int analogPin);
		double getPressure();
		void debug(bool debug);

	private:
		int _analogPin;
		bool _debug;
};

#endif