#include <iostream>
#include <Windows.h>
#include <windowsx.h> 
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

LPWAVEHDR m_lpWaveHdr1, m_lpWaveHdr2;
LPSTR m_lpBuffer1, m_lpBuffer2;

int		m_nChannels = 2; 
int		m_nSamplesPerSec = 11025; 
WORD	m_wBitsPerSample = 16; 
int		m_nBlockAlign; 
DWORD	m_dwBufferSize;

WAVEFORMATEX m_Format;
HWAVEIN 	 m_hWaveIn;
//HPSTR    	 m_lpData;
//DWORD		 m_dwDataLength;

DWORD_PTR m_hWnd;

void FreeGlobalPtr(LPVOID p)   
{   
    if (p != NULL)   
    {   
        GlobalFreePtr(p);   
        p = NULL;   
    }   
}

void JudgeSound(double* data0) {
	for(int i =1; i <= 5510; i++ ) {
		if(data0[i - 1] > 0.4) {
			cout << i - 1 << " " << data0[i-1] << endl;
			data0[i-1] = 0;
		}
	}

}

static void CALLBACK waveInProc(
  HWAVEIN hwi,       
  UINT uMsg,         
  DWORD_PTR dwInstance,  
  DWORD_PTR dwParam1,    
  DWORD_PTR dwParam2     
)
{
    //((CVoineIn*)dwInstance)->WaveInProcess(hwi,uMsg,dwInstance,dwParam1,dwParam2);
	switch(uMsg)
	{
	case WIM_DATA:
		cout<<"WIM_DATA\n";
		 {
            PWAVEHDR pWaveHdr = (PWAVEHDR)dwParam1;
            if(pWaveHdr->dwBytesRecorded > 0)
            {
				//JudgeSound((double*)dwParam1);
                //lb_BroadcastBuffer_Pushback(&broadcast_capture_frame,pWaveHdr->lpData);
				waveInPrepareHeader(hwi, pWaveHdr, sizeof(WAVEHDR));
            }
            waveInAddBuffer(hwi, pWaveHdr, sizeof (WAVEHDR));
        }
		break;
	case WIM_OPEN:
		cout<<"WIM_OPEN\n";
		break;
	case WIM_CLOSE:
		cout<<"WIM_CLOSE\n";
		break;
	default:
		cout<<"Unknown uMsg: "<<uMsg<<endl;
	}
}

BOOL RecordWave()   
{   
    // ��ѯϵͳ�Ƿ�װ�в�����Ƶ�����豸   
    if (waveInGetNumDevs() == 0)   
    {   
        // ϵͳû�в�����Ƶ�����豸   
        return FALSE;   
    }   
   
    m_nBlockAlign = m_nChannels*m_wBitsPerSample/8;   
    m_dwBufferSize = m_nSamplesPerSec*m_nBlockAlign;   //11025*4
   
    // ����¼����ʽ   
    m_Format.wFormatTag = WAVE_FORMAT_PCM;    
    m_Format.nChannels  = m_nChannels;    
    m_Format.nSamplesPerSec = m_nSamplesPerSec;    
    m_Format.nAvgBytesPerSec = m_nSamplesPerSec*m_nBlockAlign;    
    m_Format.nBlockAlign = m_nBlockAlign;    
    m_Format.wBitsPerSample = m_wBitsPerSample;    
    m_Format.cbSize = 0;//sizeof(WAVEFORMATEX);    
   
    // �򿪲�����Ƶ�豸   
    if (MMSYSERR_NOERROR !=    
                waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_Format,    
				(DWORD)(&waveInProc), NULL, CALLBACK_FUNCTION))   
    {   
        // �򿪲�����Ƶ�豸����   
        return FALSE;   
    }   
   
    // ����һ����������������ͷWAVEHDR���䡢����ȫ���ڴ�   
    m_lpWaveHdr1 = (LPWAVEHDR)GlobalAllocPtr(    
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            (DWORD) sizeof(WAVEHDR));   
    if (!m_lpWaveHdr1)   
    {   
        //����ȫ���ڴ����   
        return FALSE;   
    }   
    m_lpBuffer1 = (LPSTR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            m_dwBufferSize);   
    if (!m_lpBuffer1)   
    {   
        //����ȫ���ڴ����   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        return FALSE;   
    }   
   
    // ׼����һ���ݻ��������������豸   
    m_lpWaveHdr1->lpData = m_lpBuffer1;   
    m_lpWaveHdr1->dwBufferLength = m_dwBufferSize;   
    m_lpWaveHdr1->dwBytesRecorded = 0L;   
    m_lpWaveHdr1->dwUser = 0L;   
    m_lpWaveHdr1->dwFlags = 0L;   
    m_lpWaveHdr1->dwLoops = 1L;   
    m_lpWaveHdr1->lpNext = NULL;   
    m_lpWaveHdr1->reserved=0L;   
    if(MMSYSERR_NOERROR !=    
                waveInPrepareHeader(m_hWaveIn,    
                    m_lpWaveHdr1, sizeof(WAVEHDR)))   
    {   
        // ׼��WAVEHDR�ṹ���ݳ���   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        return FALSE;   
    }   
    waveInAddBuffer(m_hWaveIn, m_lpWaveHdr1, sizeof(WAVEHDR));   
   
    // ���ڶ�����������������ͷWAVEHDR���䡢����ȫ���ڴ�   
    m_lpWaveHdr2 = (LPWAVEHDR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            (DWORD) sizeof(WAVEHDR));   
    if (!m_lpWaveHdr2)   
    {   
        //����ȫ���ڴ����   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        return FALSE;   
    }   
    m_lpBuffer2 = (LPSTR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            m_dwBufferSize);   
    if (!m_lpBuffer2)   
    {   
        //����ȫ���ڴ����   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        FreeGlobalPtr( m_lpWaveHdr2 );   
        return FALSE;   
    }   
   
    // ׼���ڶ����ݻ��������������豸   
    m_lpWaveHdr2->lpData = m_lpBuffer2;   
    m_lpWaveHdr2->dwBufferLength = m_dwBufferSize;   
    m_lpWaveHdr2->dwBytesRecorded = 0L;   
    m_lpWaveHdr2->dwUser = 0L;   
    m_lpWaveHdr2->dwFlags = 0L;   
    m_lpWaveHdr2->dwLoops = 1L;   
    m_lpWaveHdr2->lpNext = NULL;   
    m_lpWaveHdr2->reserved=0L;   
    if(MMSYSERR_NOERROR !=    
                waveInPrepareHeader(m_hWaveIn,    
                        m_lpWaveHdr2, sizeof(WAVEHDR)))   
    {   
        // ׼��WAVEHDR�ṹ���ݳ���   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        FreeGlobalPtr( m_lpWaveHdr2 );   
        FreeGlobalPtr( m_lpBuffer2 );   
        return FALSE;   
    }   
    waveInAddBuffer(m_hWaveIn, m_lpWaveHdr2, sizeof(WAVEHDR));   
  
    // Ϊ������Ƶ����Ԥ�ȷ����ڴ棬��ʼ��СΪ1�ֽ�   
//    FreeGlobalPtr( m_lpData );   
//    m_lpData = (HPSTR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, 1);   
   
    // ��ʼ¼��   
//    m_dwDataLength = 0L;   

	cout << "Begin waveInStart" << endl;
    waveInStart(m_hWaveIn);   
   
    return TRUE;   
}

//void JudgeSound(double *data0, double *data1, int &fake) {

	/*
	//��������
	for(int i =1; i <= mdwSampleNum; i++ ) {
		if(data0[i - 1] >0.04) {
			if((mdwSampleNum - i) < 64) { //�����ź����趨�ķ�Χ
				//. . .
				num = Num(truedata, 64);
				zong = Sum(truedata, 64);
			} else { //�����źŲ����趨�ķ�Χ��
				//. . .
				num = Num(truedata, 64);
				zong = Sum(truedata, 64);
			}
		}
	}
	//. . .
	if(num >= 0 && num <= 17 && zong > 5.0) {
		fake = 0; //�����Ƿ������Ч�������ź�
	}
	*/
//}



int main() {
	cout << "Hello World!\n";

	RecordWave();	

	
	while (true) {
		JudgeSound((double*)m_lpBuffer1);
		JudgeSound((double*)m_lpBuffer2);
	}

	return 0;
}