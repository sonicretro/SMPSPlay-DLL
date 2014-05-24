// Stream.c: C Source File for Sound Output
//

// Thanks to nextvolume for NetBSD support

//#include <cstdio>
#include <malloc.h>

#ifdef WIN32
#include <windows.h>
#else
#include <limits.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#ifdef __NetBSD__
#include <sys/audioio.h>
#else
#include <linux/soundcard.h>
#endif
#include <unistd.h>

#endif

#ifdef USE_LIBAO
#ifdef WIN32
#error "Sorry, but this doesn't work yet!"
#endif
#include <ao/ao.h>
#endif

#include "chips/mamedef.h"	// for UINT8 etc.
#include "stdbool.h"
#include "Stream.h"
#include "Sound.h"

#ifndef WIN32
typedef struct
{
	UINT16 wFormatTag;
	UINT16 nChannels;
	UINT32 nSamplesPerSec;
	UINT32 nAvgBytesPerSec;
	UINT16 nBlockAlign;
	UINT16 wBitsPerSample;
	UINT16 cbSize;
} WAVEFORMATEX;	// from MSDN Help

#define WAVE_FORMAT_PCM	0x0001

#endif

#ifdef WIN32
static DWORD WINAPI WaveOutThread(void* Arg);
static void BufCheck(void);
#else
void WaveOutCallbackFnc(void);
#endif

UINT16 AUDIOBUFFERU = AUDIOBUFFERS;		// used AudioBuffers

WAVEFORMATEX WaveFmt;
extern UINT32 SampleRate;
volatile bool PauseThread;
volatile bool StreamPause;
bool ThreadPauseEnable;
volatile bool ThreadPauseConfrm;

#ifdef WIN32
static HWAVEOUT hWaveOut;
static WAVEHDR WaveHdrOut[AUDIOBUFFERS];
static HANDLE hWaveOutThread;
//static DWORD WaveOutCallbackThrID;
#else
static INT32 hWaveOut;
#endif
static bool WaveOutOpen;
UINT32 BUFFERSIZE;	// Buffer Size in Bytes
UINT32 SMPL_P_BUFFER;
static char BufferOut[AUDIOBUFFERS][BUFSIZE_MAX];
volatile bool CloseThread;


UINT32 BlocksSent;
UINT32 BlocksPlayed;

#ifdef USE_LIBAO
ao_device* dev_ao;
#endif

UINT8 StartStream(UINT8 DeviceID)
{
	UINT32 RetVal;
#ifdef USE_LIBAO
	ao_sample_format ao_fmt;
#else
#ifdef WIN32
	UINT16 Cnt;
	HANDLE WaveOutThreadHandle;
	DWORD WaveOutThreadID;
	//char TestStr[0x80];
#elif defined(__NetBSD__)
	struct audio_info AudioInfo;
#else
	UINT32 ArgVal;
#endif
#endif	// ! USE_LIBAO
	
	if (WaveOutOpen)
		return 0xD0;	// Thread is already active
	
	// Init Audio
	WaveFmt.wFormatTag = WAVE_FORMAT_PCM;
	WaveFmt.nChannels = 2;
	WaveFmt.nSamplesPerSec = SampleRate;
	WaveFmt.wBitsPerSample = 16;
	WaveFmt.nBlockAlign = WaveFmt.wBitsPerSample * WaveFmt.nChannels / 8;
	WaveFmt.nAvgBytesPerSec = WaveFmt.nSamplesPerSec * WaveFmt.nBlockAlign;
	WaveFmt.cbSize = 0;
	if (DeviceID == 0xFF)
		return 0x00;
	
#if defined(WIN32) || defined(USE_LIBAO)
	BUFFERSIZE = SampleRate / 100 * SAMPLESIZE;
	if (BUFFERSIZE > BUFSIZE_MAX)
		BUFFERSIZE = BUFSIZE_MAX;
#else
	BUFFERSIZE = 1 << BUFSIZELD;
#endif
	SMPL_P_BUFFER = BUFFERSIZE / SAMPLESIZE;
	if (AUDIOBUFFERU > AUDIOBUFFERS)
		AUDIOBUFFERU = AUDIOBUFFERS;
	
	PauseThread = true;
	ThreadPauseConfrm = false;
	CloseThread = false;
	StreamPause = false;
	
#ifndef USE_LIBAO
#ifdef WIN32
	ThreadPauseEnable = true;
	WaveOutThreadHandle = CreateThread(NULL, 0x00, &WaveOutThread, NULL, 0x00,
										&WaveOutThreadID);
	if(WaveOutThreadHandle == NULL)
		return 0xC8;		// CreateThread failed
	CloseHandle(WaveOutThreadHandle);
	
	RetVal = waveOutOpen(&hWaveOut, ((UINT)DeviceID - 1), &WaveFmt, 0x00, 0x00, CALLBACK_NULL);
	if(RetVal != MMSYSERR_NOERROR)
#else
	ThreadPauseEnable = false;
#ifdef __NetBSD__
	hWaveOut = open("/dev/audio", O_WRONLY);
#else
	hWaveOut = open("/dev/dsp", O_WRONLY);
#endif
	if (hWaveOut < 0)
#endif
#else	// ifdef USE_LIBAO
	ao_initialize();
	
	ThreadPauseEnable = false;
	ao_fmt.bits = WaveFmt.wBitsPerSample;
	ao_fmt.rate = WaveFmt.nSamplesPerSec;
	ao_fmt.channels = WaveFmt.nChannels;
	ao_fmt.byte_format = AO_FMT_NATIVE;
	ao_fmt.matrix = NULL;
	
	dev_ao = ao_open_live(ao_default_driver_id(), &ao_fmt, NULL);
	if (dev_ao == NULL)
#endif
	{
		CloseThread = true;
		return 0xC0;		// waveOutOpen failed
	}
	WaveOutOpen = true;
	
	//sprintf(TestStr, "Buffer 0,0:\t%p\nBuffer 0,1:\t%p\nBuffer 1,0:\t%p\nBuffer 1,1:\t%p\n",
	//		&BufferOut[0][0], &BufferOut[0][1], &BufferOut[1][0], &BufferOut[1][1]);
	//AfxMessageBox(TestStr);
#ifndef USE_LIBAO
#ifdef WIN32
	for (Cnt = 0x00; Cnt < AUDIOBUFFERU; Cnt ++)
	{
		WaveHdrOut[Cnt].lpData = BufferOut[Cnt];	// &BufferOut[Cnt][0x00];
		WaveHdrOut[Cnt].dwBufferLength = BUFFERSIZE;
		WaveHdrOut[Cnt].dwBytesRecorded = 0x00;
		WaveHdrOut[Cnt].dwUser = 0x00;
		WaveHdrOut[Cnt].dwFlags = 0x00;
		WaveHdrOut[Cnt].dwLoops = 0x00;
		WaveHdrOut[Cnt].lpNext = NULL;
		WaveHdrOut[Cnt].reserved = 0x00;
		RetVal = waveOutPrepareHeader(hWaveOut, &WaveHdrOut[Cnt], sizeof(WAVEHDR));
		WaveHdrOut[Cnt].dwFlags |= WHDR_DONE;
	}
#elif defined(__NetBSD__)
	AUDIO_INITINFO(&AudioInfo);
	
	AudioInfo.mode = AUMODE_PLAY;
	AudioInfo.play.sample_rate = WaveFmt.nSamplesPerSec;
	AudioInfo.play.channels = WaveFmt.nChannels;
	AudioInfo.play.precision = WaveFmt.wBitsPerSample;
	AudioInfo.play.encoding = AUDIO_ENCODING_SLINEAR;
	
	RetVal = ioctl(hWaveOut, AUDIO_SETINFO, &AudioInfo);
	if (RetVal)
		printf("Error setting audio information!\n");
#else
	ArgVal = (AUDIOBUFFERU << 16) | BUFSIZELD;
	RetVal = ioctl(hWaveOut, SNDCTL_DSP_SETFRAGMENT, &ArgVal);
	if (RetVal)
		printf("Error setting Fragment Size!\n");
	ArgVal = AFMT_S16_NE;
	RetVal = ioctl(hWaveOut, SNDCTL_DSP_SETFMT, &ArgVal);
	if (RetVal)
		printf("Error setting Format!\n");
	ArgVal = WaveFmt.nChannels;
	RetVal = ioctl(hWaveOut, SNDCTL_DSP_CHANNELS, &ArgVal);
	if (RetVal)
		printf("Error setting Channels!\n");
	ArgVal = WaveFmt.nSamplesPerSec;
	RetVal = ioctl(hWaveOut, SNDCTL_DSP_SPEED, &ArgVal);
	if (RetVal)
		printf("Error setting Sample Rate!\n");
#endif
#endif	// USE_LIBAO
	
	PauseThread = false;
	
	return 0x00;
}

UINT8 StopStream(void)
{
	UINT32 RetVal;
#ifdef WIN32
	UINT16 Cnt;
#endif
	
	if (! WaveOutOpen)
		return 0xD8;	// Thread is not active
	
	CloseThread = true;
#ifdef WIN32
	for (Cnt = 0; Cnt < 100; Cnt ++)
	{
		Sleep(1);
		if (hWaveOutThread == NULL)
			break;
	}
#endif
	WaveOutOpen = false;
	
#ifndef USE_LIBAO
#ifdef WIN32
	RetVal = waveOutReset(hWaveOut);
	for (Cnt = 0x00; Cnt < AUDIOBUFFERU; Cnt ++)
		RetVal = waveOutUnprepareHeader(hWaveOut, &WaveHdrOut[Cnt], sizeof(WAVEHDR));
	
	RetVal = waveOutClose(hWaveOut);
	if(RetVal != MMSYSERR_NOERROR)
		return 0xC4;		// waveOutClose failed  -- but why ???
#else
	close(hWaveOut);
#endif
#else	// ifdef USE_LIBAO
	ao_close(dev_ao);
	
	ao_shutdown();
#endif
	
	return 0x00;
}

void PauseStream(bool PauseOn)
{
	UINT32 RetVal;
	
	if (! WaveOutOpen)
		return;	// Thread is not active
	
#ifdef WIN32
	switch(PauseOn)
	{
	case true:
		RetVal = waveOutPause(hWaveOut);
		break;
	case false:
		RetVal = waveOutRestart(hWaveOut);
		break;
	}
	StreamPause = PauseOn;
#else
	PauseThread = PauseOn;
#endif
	
	return;
}

#ifdef WIN32

static DWORD WINAPI WaveOutThread(void* Arg)
{
#ifdef NDEBUG
	UINT32 RetVal;
#endif
	UINT16 CurBuf;
	WAVE_16BS* TempBuf;
	UINT32 WrtSmpls;
	//char TestStr[0x80];
	bool DidBuffer;	// a buffer was processed
	
	hWaveOutThread = GetCurrentThread();
#ifdef NDEBUG
	RetVal = SetThreadPriority(hWaveOutThread, THREAD_PRIORITY_TIME_CRITICAL);
	if (! RetVal)
	{
		// Error by setting priority
		// try a lower priority, because too low priorities cause sound stuttering
		RetVal = SetThreadPriority(hWaveOutThread, THREAD_PRIORITY_HIGHEST);
	}
#endif
	
	BlocksSent = 0x00;
	BlocksPlayed = 0x00;
	while(! CloseThread)
	{
		while(PauseThread && ! CloseThread && ! (StreamPause && DidBuffer))
		{
			ThreadPauseConfrm = true;
			Sleep(1);
		}
		if (CloseThread)
			break;
		
		BufCheck();
		DidBuffer = false;
		for (CurBuf = 0x00; CurBuf < AUDIOBUFFERU; CurBuf ++)
		{
			if (WaveHdrOut[CurBuf].dwFlags & WHDR_DONE)
			{
				TempBuf = (WAVE_16BS*)WaveHdrOut[CurBuf].lpData;
				
				if (WaveHdrOut[CurBuf].dwUser & 0x01)
					BlocksPlayed ++;
				else
					WaveHdrOut[CurBuf].dwUser |= 0x01;
				
				WrtSmpls = FillBuffer(TempBuf, SMPL_P_BUFFER);
				
				WaveHdrOut[CurBuf].dwBufferLength = WrtSmpls * SAMPLESIZE;
				waveOutWrite(hWaveOut, &WaveHdrOut[CurBuf], sizeof(WAVEHDR));
				
				DidBuffer = true;
				BlocksSent ++;
				BufCheck();
				//CurBuf = 0x00;
				//break;
			}
			if (CloseThread)
				break;
		}
		Sleep(1);
	}
	
	hWaveOutThread = NULL;
	return 0x00000000;
}

static void BufCheck(void)
{
	UINT16 CurBuf;
	
	for (CurBuf = 0x00; CurBuf < AUDIOBUFFERU; CurBuf ++)
	{
		if (WaveHdrOut[CurBuf].dwFlags & WHDR_DONE)
		{
			if (WaveHdrOut[CurBuf].dwUser & 0x01)
			{
				WaveHdrOut[CurBuf].dwUser &= ~0x01;
				BlocksPlayed ++;
			}
		}
	}
	
	return;
}

#else	// #ifndef WIN32

void WaveOutLinuxCallBack(void)
{
	UINT32 RetVal;
	UINT16 CurBuf;
	WAVE_16BS* TempBuf;
	UINT32 WrtSmpls;
	
	if (! WaveOutOpen)
		return;	// Device not opened
	
	CurBuf = BlocksSent % AUDIOBUFFERU;
	TempBuf = (WAVE_16BS*)BufferOut[CurBuf];
	
	WrtSmpls = FillBuffer(TempBuf, SMPL_P_BUFFER);
	
#ifndef USE_LIBAO
	RetVal = write(hWaveOut, TempBuf, WrtSmpls * SAMPLESIZE);
#else
	RetVal = ao_play(dev_ao, (char*)TempBuf, WrtSmpls * SAMPLESIZE);
#endif
	BlocksSent ++;
	BlocksPlayed ++;
	
	return;
}

#endif
