#include <Windows.h>
#include "Sounder.h"

const int DURATION = 5000;//10 min

Sounder::Sounder(int theFrequency)
{
	//没有处理范围
	frequency = theFrequency;
}

void BeepHelper(LPVOID lpParamter)
{
	Beep((int)lpParamter,DURATION);//
}

void Sounder::start()
{
	soundThread = CreateThread(NULL,								//SD
							   0,									//initial stack size
							   (LPTHREAD_START_ROUTINE)BeepHelper,	//thread function
							   (LPVOID)frequency,					//thread argument
							   0,									//creation option
							   NULL);								//thread identifier
}

void Sounder::turnOff()
{
	TerminateThread(soundThread,0);
}

