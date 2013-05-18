#include <iostream>
#include "Recorder.h"
#include "Sounder.h"
#include "Fourier.h"


using namespace std;

BOOL Process(void* lpData, LPWAVEHDR pwh)
{
#define FFT_LEN 2048

	//double finleft[FFT_LEN/2],finright[FFT_LEN/2],fout[FFT_LEN],foutimg[FFT_LEN],fdraw[FFT_LEN/2];
	DWORD nCount = 0;
	for (DWORD dw = 0; dw < FFT_LEN; dw++)
	{
		{
			//copy audio signal to fft real component for left channel
			//finleft[nCount] = (double)((short*)pwh->lpData)[dw++];
			//copy audio signal to fft real component for right channel
			//finright[nCount++] = (double)((short*)pwh->lpData)[dw];
			double value = (double)((short*)pwh->lpData)[dw];
			if(value > 10000)
			cout<<value<<endl;
		}
	}
	
	return TRUE;
}

int main() {
	Recorder m_rec;
	Sounder m_sounder(1000);//1000Hz

	m_sounder.start();

	m_rec.Open();
	m_rec.SetBufferFunction(NULL,Process);
	m_rec.Start();

	while(1){}

	return 0;
}