/***************************************************
 * To Do List: 1. include our sounder to our project
 *             2. calculate the difference or change
 *                of fft_db ACCURATELY
 *
****************************************************/



#include <iostream>
#include "Recorder.h"
#include "Sounder.h"
#include "Fourier.h"
#include <WinUser.h>

using namespace std;

#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))


BOOL Process(void* lpData, LPWAVEHDR pwh)
{
#define FFT_LEN 2048

	DWORD nCount = 0;
	double fftin[FFT_LEN/2],fout[FFT_LEN],foutimg[FFT_LEN],fft_db[FFT_LEN/2];
	for (DWORD dw = 0; dw < FFT_LEN; dw++)
	{
		{
			double value = (double)((short*)pwh->lpData)[dw++];
			fftin[nCount++] = value;

			//if( value > 1000)cout<<value<<endl;
		}
		
	}
	
	/***  Perform FFT  ***/
	fft_double(FFT_LEN/2.0,0,fftin,NULL,fout,foutimg);
	float re,im;
	for(int i=1;i<FFT_LEN/4;i++)
	{
		re = (float)fout[i];
		im = (float)foutimg[i];

		fft_db[i] = Decibels(re,im);
	}


	/****  For Debugging  ****/
	//for(int i=400;i<FFT_LEN/4;i++)
	//{
	//	if(fft_db[i] >= 103)
	//		cout<<i<<" : "<<fft_db[i]<<endl;
	//}
	/****  For Debugging  ****/


	/***  Mouse Left Down and Up Event  ***/
	/*  NOTE: just very simple condition. 
	 *  487 and 488 in fft_db is about in the range of ultrasound
	 *  The frequency of the Kth index of fft_db is 44100*K/1024
	 *  The next step to do is to calculate the difference of fft_db ACCURATELY
	 */

	if(fft_db[464] >=104 || fft_db[465] >= 104)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	}

	return TRUE;
}

int main() {
	Recorder m_rec;

	/*  NOTE: The next step is to include our sounder into this project.
	 */

	Sounder m_sounder(20000);//1000Hz

	m_sounder.start();

	m_rec.Open();
	m_rec.SetBufferFunction(NULL,Process);
	m_rec.Start();

	while(1){}

	return 0;
}