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
    // 查询系统是否安装有波形音频输入设备   
    if (waveInGetNumDevs() == 0)   
    {   
        // 系统没有波形音频输入设备   
        return FALSE;   
    }   
   
    m_nBlockAlign = m_nChannels*m_wBitsPerSample/8;   
    m_dwBufferSize = m_nSamplesPerSec*m_nBlockAlign;   //11025*4
   
    // 设置录音格式   
    m_Format.wFormatTag = WAVE_FORMAT_PCM;    
    m_Format.nChannels  = m_nChannels;    
    m_Format.nSamplesPerSec = m_nSamplesPerSec;    
    m_Format.nAvgBytesPerSec = m_nSamplesPerSec*m_nBlockAlign;    
    m_Format.nBlockAlign = m_nBlockAlign;    
    m_Format.wBitsPerSample = m_wBitsPerSample;    
    m_Format.cbSize = 0;//sizeof(WAVEFORMATEX);    
   
    // 打开波形音频设备   
    if (MMSYSERR_NOERROR !=    
                waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_Format,    
				(DWORD)(&waveInProc), NULL, CALLBACK_FUNCTION))   
    {   
        // 打开波形音频设备出错   
        return FALSE;   
    }   
   
    // 给第一缓冲区及波形数据头WAVEHDR分配、锁定全局内存   
    m_lpWaveHdr1 = (LPWAVEHDR)GlobalAllocPtr(    
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            (DWORD) sizeof(WAVEHDR));   
    if (!m_lpWaveHdr1)   
    {   
        //分配全局内存出错   
        return FALSE;   
    }   
    m_lpBuffer1 = (LPSTR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            m_dwBufferSize);   
    if (!m_lpBuffer1)   
    {   
        //分配全局内存出错   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        return FALSE;   
    }   
   
    // 准备第一数据缓冲区，并送入设备   
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
        // 准备WAVEHDR结构数据出错   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        return FALSE;   
    }   
    waveInAddBuffer(m_hWaveIn, m_lpWaveHdr1, sizeof(WAVEHDR));   
   
    // 给第二缓冲区及波形数据头WAVEHDR分配、锁定全局内存   
    m_lpWaveHdr2 = (LPWAVEHDR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            (DWORD) sizeof(WAVEHDR));   
    if (!m_lpWaveHdr2)   
    {   
        //分配全局内存出错   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        return FALSE;   
    }   
    m_lpBuffer2 = (LPSTR)GlobalAllocPtr(   
                            GMEM_MOVEABLE | GMEM_SHARE,   
                            m_dwBufferSize);   
    if (!m_lpBuffer2)   
    {   
        //分配全局内存出错   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        FreeGlobalPtr( m_lpWaveHdr2 );   
        return FALSE;   
    }   
   
    // 准备第二数据缓冲区，并送入设备   
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
        // 准备WAVEHDR结构数据出错   
        FreeGlobalPtr( m_lpWaveHdr1 );   
        FreeGlobalPtr( m_lpBuffer1 );   
        FreeGlobalPtr( m_lpWaveHdr2 );   
        FreeGlobalPtr( m_lpBuffer2 );   
        return FALSE;   
    }   
    waveInAddBuffer(m_hWaveIn, m_lpWaveHdr2, sizeof(WAVEHDR));   
  
    // 为波形音频数据预先分配内存，初始大小为1字节   
//    FreeGlobalPtr( m_lpData );   
//    m_lpData = (HPSTR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, 1);   
   
    // 开始录音   
//    m_dwDataLength = 0L;   

	cout << "Begin waveInStart" << endl;
    waveInStart(m_hWaveIn);   
   
    return TRUE;   
}

//void JudgeSound(double *data0, double *data1, int &fake) {

	/*
	//变量声明
	for(int i =1; i <= mdwSampleNum; i++ ) {
		if(data0[i - 1] >0.04) {
			if((mdwSampleNum - i) < 64) { //声音信号在设定的范围
				//. . .
				num = Num(truedata, 64);
				zong = Sum(truedata, 64);
			} else { //声音信号不在设定的范围。
				//. . .
				num = Num(truedata, 64);
				zong = Sum(truedata, 64);
			}
		}
	}
	//. . .
	if(num >= 0 && num <= 17 && zong > 5.0) {
		fake = 0; //返回是否包含有效的声音信号
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