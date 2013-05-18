//ultrasound generator

#ifndef SOUNDER_H
#define SOUNDER_H

#include <Windows.h>

class Sounder
{
public:
	Sounder(int theFrequency);

	void start();//the sounder starts to make sound
	void turnOff();//turn off the sounder
private:
	//LPTHREAD_START_ROUTINE BeepHelper(LPVOID lpParamter);//make the sound of beep
	int frequency; //frequency the sounder generates
	HANDLE soundThread;//the HANDLER for the sound thread
};

#endif