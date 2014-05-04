// SMPSOUT.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Engine\smps.h"
#include "Engine\smps_commands.h"
#include "Sound.h"
#include "Stream.h"
#include <sstream>
#include "resource.h"

HMODULE moduleHandle;
HWND gameWindow;
extern volatile bool PauseThread;	// from Stream.c
extern volatile bool ThreadPauseConfrm;
extern volatile bool CloseThread;
extern UINT16 FrameDivider;

struct dacentry { int id; int resid; unsigned char rate; };

dacentry DACFiles[] = {
	{ 0x81, IDR_DAC_81, 0x04 },
	{ 0x82, IDR_DAC_82, 0x0E },
	{ 0x83, IDR_DAC_82, 0x14 },
	{ 0x84, IDR_DAC_82, 0x1A },
	{ 0x85, IDR_DAC_82, 0x20 },
	{ 0x86, IDR_DAC_86, 0x04 },
	{ 0x87, IDR_DAC_87, 0x04 },
	{ 0x88, IDR_DAC_88, 0x06 },
	{ 0x89, IDR_DAC_89, 0x0A },
	{ 0x8A, IDR_DAC_8A, 0x14 },
	{ 0x8B, IDR_DAC_8A, 0x1B },
	{ 0x8C, IDR_DAC_8C, 0x08 },
	{ 0x8D, IDR_DAC_8D, 0x0B },
	{ 0x8E, IDR_DAC_8D, 0x11 },
	{ 0x8F, IDR_DAC_8F, 0x08 },
	{ 0x90, IDR_DAC_90, 0x03 },
	{ 0x91, IDR_DAC_90, 0x07 },
	{ 0x92, IDR_DAC_90, 0x0A },
	{ 0x93, IDR_DAC_90, 0x0E },
	{ 0x94, IDR_DAC_94, 0x06 },
	{ 0x95, IDR_DAC_94, 0x0A },
	{ 0x96, IDR_DAC_94, 0x0D },
	{ 0x97, IDR_DAC_94, 0x12 },
	{ 0x98, IDR_DAC_98, 0x0B },
	{ 0x99, IDR_DAC_98, 0x13 },
	{ 0x9A, IDR_DAC_98, 0x16 },
	{ 0x9B, IDR_DAC_9B, 0x0C },
	{ 0x9C, IDR_DAC_9C, 0x0A },
	{ 0x9D, IDR_DAC_9D, 0x18 },
	{ 0x9E, IDR_DAC_9E, 0x18 },
	{ 0x9F, IDR_DAC_9F, 0x0C },
	{ 0xA0, IDR_DAC_A0, 0x0C },
	{ 0xA1, IDR_DAC_A1, 0x0A },
	{ 0xA2, IDR_DAC_A2, 0x0A },
	{ 0xA3, IDR_DAC_A3, 0x18 },
	{ 0xA4, IDR_DAC_A4, 0x18 },
	{ 0xA5, IDR_DAC_A5, 0x0C },
	{ 0xA6, IDR_DAC_A6, 0x09 },
	{ 0xA7, IDR_DAC_A7, 0x18 },
	{ 0xA8, IDR_DAC_A8, 0x18 },
	{ 0xA9, IDR_DAC_A9, 0x0C },
	{ 0xAA, IDR_DAC_AA, 0x0A },
	{ 0xAB, IDR_DAC_AB, 0x0D },
	{ 0xAC, IDR_DAC_AC, 0x06 },
	{ 0xAD, IDR_DAC_AD, 0x10 },
	{ 0xAE, IDR_DAC_AD, 0x18 },
	{ 0xAF, IDR_DAC_AF, 0x09 },
	{ 0xB0, IDR_DAC_AF, 0x12 },
	{ 0xB1, IDR_DAC_B1, 0x18 },
	{ 0xB2, IDR_DAC_B2, 0x16 },
	{ 0xB3, IDR_DAC_B2, 0x20 },
	{ 0xB4, IDR_DAC_B4, 0x0C },
	{ 0xB5, IDR_DAC_B5, 0x0C },
	{ 0xB6, IDR_DAC_B6, 0x0C },
	{ 0xB7, IDR_DAC_B7, 0x18 },
	{ 0xB8, IDR_DAC_B8, 0x0C },
	{ 0xB9, IDR_DAC_B8, 0x0C },
	{ 0xBA, IDR_DAC_BA, 0x18 },
	{ 0xBB, IDR_DAC_BB, 0x18 },
	{ 0xBC, IDR_DAC_BC, 0x18 },
	{ 0xBD, IDR_DAC_BD, 0x0C },
	{ 0xBE, IDR_DAC_BE, 0x0C },
	{ 0xBF, IDR_DAC_BF, 0x1C },
	{ 0xC0, IDR_DAC_C0, 0x0B },
	{ 0xC1, IDR_DAC_B4, 0x0F },
	{ 0xC2, IDR_DAC_B4, 0x11 },
	{ 0xC3, IDR_DAC_B4, 0x12 },
	{ 0xC4, IDR_DAC_B4, 0x0B },
};

struct musicentry { unsigned short base; bool s3; };

musicentry MusicFiles[] = {
	{ 0xE18F, true }, // 1
	{ 0x8000, true }, // 2
	{ 0x9B6D, true }, // 3
	{ 0xB0BC, true }, // 4
	{ 0xC0C6, true }, // 5
	{ 0xD364, true }, // 6
	{ 0xD97B, true }, // 7
	{ 0xE48F, true }, // 8
	{ 0xDDA9, true }, // 9
	{ 0x8000, false }, // 10
	{ 0x8597, false }, // 11
	{ 0x86AA, true }, // 12
	{ 0x8000, true }, // 13
	{ 0x90CF, true }, // 14
	{ 0x8DC8, true }, // 15
	{ 0x8AFE, false }, // 16
	{ 0x9106, false }, // 17
	{ 0x9688, false }, // 18
	{ 0x9CF2, false }, // 19
	{ 0xA2E5, false }, // 20
	{ 0xACF3, false }, // 21
	{ 0xBE80, false }, // 22
	{ 0xC2B4, false }, // 23
	{ 0xC79F, false }, // 24
	{ 0xCBB1, false }, // 25
	{ 0xCEE1, false }, // 26
	{ 0xD3DD, false }, // 27
	{ 0xDCC0, false }, // 28
	{ 0xE223, false }, // 29
	{ 0xEABB, false }, // 30
	{ 0x8AE8, true }, // 31
	{ 0x97FD, true }, // 32
	{ 0x99F7, true }, // 33
	{ 0xA4FD, true }, // 34
	{ 0xB0EC, true }, // 35
	{ 0xC324, true }, // 36
	{ 0xDA47, true }, // 37
	{ 0xDD4B, false }, // 38
	{ 0xDFA6, false }, // 39
	{ 0xE3C0, false }, // 40
	{ 0xF095, true }, // 41
	{ 0xFE75, false }, // 42
	{ 0xF364, true }, // 43
	{ 0xF5E4, true }, // 44
	{ 0xF1A0, true }, // 45
	{ 0xE7AF, false }, // 46
	{ 0xF74C, false }, // 47
	{ 0xFABE, false }, // 48
	{ 0xFBFE, true }, // 49
	{ 0xE587, true }, // 50
	{ 0xF5A3, false }, // 51
	{ 0xF88E, false }, // 52
	{ 0xFD4B, false }, // 53
	{ 0xE574, false }, // 54
	{ 0xFCDE, false }, // 55
	{ 0xC104, false } // 56
};

static const UINT8 DefDPCMData[] =
{	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
0x80, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0};

static const UINT8 FMCHN_ORDER[] = {0x16, 0, 1, 2, 4, 5, 6};

static const UINT16 DEF_FMFREQ_VAL[] =
{0x284, 0x2AB, 0x2D3, 0x2FE, 0x32D, 0x35C, 0x38F, 0x3C5, 0x3FF, 0x43C, 0x47C, 0x4C0};

static const UINT16 DEF_PSGFREQ_Z80_T2_VAL[] =
{	0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3F7, 0x3BE, 0x388,
0x356, 0x326, 0x2F9, 0x2CE, 0x2A5, 0x280, 0x25C, 0x23A, 0x21A, 0x1FB, 0x1DF, 0x1C4,
0x1AB, 0x193, 0x17D, 0x167, 0x153, 0x140, 0x12E, 0x11D, 0x10D, 0x0FE, 0x0EF, 0x0E2,
0x0D6, 0x0C9, 0x0BE, 0x0B4, 0x0A9, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071,
0x06B, 0x065, 0x05F, 0x05A, 0x055, 0x050, 0x04B, 0x047, 0x043, 0x040, 0x03C, 0x039,
0x036, 0x033, 0x030, 0x02D, 0x02B, 0x028, 0x026, 0x024, 0x022, 0x020, 0x01F, 0x01D,
0x01B, 0x01A, 0x018, 0x017, 0x016, 0x015, 0x013, 0x012, 0x011, 0x010, 0x000, 0x000};

static const UINT16 FM3FREQS[] = { 0, 0x132, 0x18E, 0x1E4, 0x234, 0x27E, 0x2C2, 0x2F0 };

static const CMD_FLAGS CMDFLAGS[] = {
	{ CF_PANAFMS, CFS_PAFMS_PAN, 0x02 },
	{ CF_DETUNE, 0x00, 0x02 },
	{ CF_FADE_IN, 0x00, 0x02 },
	{ CF_TRK_END, CFS_TEND_MUTE, 0x01 },
	{ CF_VOLUME, CFS_VOL_ABS_S3K, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_FMP2, 0x03 },
	{ CF_VOLUME, CFS_VOL_CC_FM, 0x02 },
	{ CF_HOLD, 0x00, 0x01 },
	{ CF_NOTE_STOP, CFS_NSTOP_MULT, 0x02 },
	{ CF_IGNORE, 0x00, 0x01 },
	{ CF_PLAY_DAC, 0x00, 0x02 },
	{ CF_LOOP_EXIT, 0x00, 0x04, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_PSG, 0x02 },
	{ CF_TRANSPOSE, CFS_TRNSP_SET_S3K, 0x02 },
	{ CF_FM_COMMAND, 0x00, 0x03 },
	{ CF_INSTRUMENT, CFS_INS_C_FM, 0x82 },
	{ CF_MOD_SETUP, 0x00, 0x05 },
	{ CF_MOD_ENV, CFS_MENV_FMP, 0x03 },
	{ CF_TRK_END, CFS_TEND_STD, 0x01 },
	{ CF_PSG_NOISE, CFS_PNOIS_SRES, 0x02 },
	{ CF_MOD_ENV, CFS_MENV_GEN, 0x02 },
	{ CF_INSTRUMENT, CFS_INS_C_PSG, 0x02 },
	{ CF_GOTO, 0x00, 0x03, 0x01 },
	{ CF_LOOP, 0x00, 0x05, 0x03 },
	{ CF_GOSUB, 0x00, 0x03, 0x01 },
	{ CF_RETURN, 0x00, 0x01 },
	{ CF_MOD_SET, CFS_MODS_OFF, 0x01 },
	{ CF_TRANSPOSE, CFS_TRNSP_ADD, 0x02 },
	{ CF_IGNORE, 0x00, 0x03, 0x01 },
	{ CF_RAW_FREQ, 0x00, 0x02 },
	{ CF_SPC_FM3, 0x00, 0x05 },
	{ CF_META_CF, 0x00, 0x01 }
};

const CMD_FLAGS CMDMETAFLAGS[] = {
	{ CF_TEMPO, CFS_TEMPO_SET, 0x02 },
	{ CF_SND_CMD, 0x00, 0x02 },
	{ CF_IGNORE, 0x00, 0x02 },
	{ CF_IGNORE, 0x00, 0x03 },
	{ CF_TICK_MULT, CFS_TMULT_ALL, 0x02 },
	{ CF_SSG_EG, CFS_SEG_NORMAL, 0x05 },
	{ CF_FM_VOLENV, 0x00, 0x03 },
	{ CF_IGNORE, 0x00, 0x01 }
};

static const UINT8 INSOPS_DEFAULT[] =
{	0xB0,
0x30, 0x38, 0x34, 0x3C,
0x50, 0x58, 0x54, 0x5C,
0x60, 0x68, 0x64, 0x6C,
0x70, 0x78, 0x74, 0x7C,
0x80, 0x88, 0x84, 0x8C,
0x40, 0x48, 0x44, 0x4C};

template <typename T, size_t N>
inline size_t LengthOfArray(const T(&)[N])
{
	return N;
}

unsigned int &Sonic3Mode = *(unsigned int *)0x831180;

class MidiInterfaceClass
{
public:
	virtual BOOL initialize(HWND hwnd) = 0; // hwnd = game window
	virtual BOOL load_song(unsigned char id, unsigned int bgmmode) = 0; // id = song to be played + 1 (well, +1 compared to the sound test id, it's the ID of the song in the MIDIOUT.DLL's resources); bgmmode = 0 for FM synth, 1 for General MIDI
	virtual BOOL play_song() = 0;
	virtual BOOL stop_song() = 0;
	virtual BOOL pause_song() = 0;
	virtual BOOL unpause_song() = 0;
	virtual BOOL set_song_tempo(unsigned int pct) = 0; // pct = percentage of delay between beats the song should be set to. lower = faster tempo
};

class SMPSInterfaceClass : MidiInterfaceClass
{
	SMPS_CFG smpscfg;
	bool playerRunning;
	bool s3mode;

	static void LoadRegisterList(SMPS_CFG* SmpsCfg)
	{
		UINT8 RegCnt;
		UINT8 CurReg;
		const UINT8* RegPtr;
		UINT8* RegList;
		UINT8 RegTL_Idx;

		if (SmpsCfg->InsRegs != NULL)
			free(SmpsCfg->InsRegs);

		RegCnt = LengthOfArray(INSOPS_DEFAULT);
		RegPtr = INSOPS_DEFAULT;

		RegTL_Idx = 0xFF;
		for (CurReg = 0x00; CurReg < RegCnt; CurReg ++)
		{
			if (RegPtr[CurReg] == 0x00 || (RegPtr[CurReg] & 0x03))
				break;
			if ((RegPtr[CurReg] & 0xF0) == 0x40 && RegTL_Idx == 0xFF)
				RegTL_Idx = CurReg;
		}
		RegCnt = CurReg;
		if (! RegCnt)
		{
			SmpsCfg->InsRegCnt = 0x00;
			SmpsCfg->InsRegs = NULL;
			SmpsCfg->InsReg_TL = NULL;
			return;
		}

		RegList = (UINT8*)malloc(RegCnt + 1);
		memcpy(RegList, RegPtr, RegCnt);
		RegList[RegCnt] = 0x00;

		SmpsCfg->InsRegCnt = RegCnt;
		SmpsCfg->InsRegs = RegList;
		if (RegTL_Idx == 0xFF)
			SmpsCfg->InsReg_TL = NULL;
		else
			SmpsCfg->InsReg_TL = &RegList[RegTL_Idx];

		return;
	}

	static void GetEnvelopeData(std::istream &str, ENV_LIB *EnvLib)
	{
		UINT8 CurEnv;
		ENV_DATA* TempEnv;

		str.seekg(7);
		EnvLib->EnvCount = (UINT8)str.get();
		EnvLib->EnvData = new ENV_DATA[EnvLib->EnvCount];
		for (CurEnv = 0x00; CurEnv < EnvLib->EnvCount; CurEnv ++)
		{
			TempEnv = &EnvLib->EnvData[CurEnv];
			str.seekg(str.get(), std::istringstream::cur);

			TempEnv->Len = (UINT8)str.get();
			TempEnv->Data = new UINT8[TempEnv->Len];
			str.read((char *)TempEnv->Data, TempEnv->Len);
		}
	}

INLINE UINT16 ReadBE16(const UINT8* Data)
{
	return (Data[0x00] << 8) | (Data[0x01] << 0);
}

INLINE UINT16 ReadLE16(const UINT8* Data)
{
	return (Data[0x01] << 8) | (Data[0x00] << 0);
}

INLINE UINT16 ReadRawPtr(const UINT8* Data, const SMPS_CFG* SmpsCfg)
{
	if ((SmpsCfg->PtrFmt & PTRFMT_EMASK) == PTRFMT_BE)
		return ReadBE16(Data);
	else
		return ReadLE16(Data);
}

INLINE UINT16 ReadPtr(const UINT8* Data, const SMPS_CFG* SmpsCfg)
{
	return ReadRawPtr(Data, SmpsCfg) - SmpsCfg->SeqBase;
}

static void PreparseSMPSFile(SMPS_CFG* SmpsCfg)
{
	UINT32 FileLen;
	UINT8* FileData;
	CMD_LIB* CmdList;
	CMD_LIB* CmdMetaList;
	DAC_CFG* DACDrv;
	UINT16 InsPtr;
	UINT8 FMTrkCnt;
	UINT8 PSGTrkCnt;
	UINT8 TrkCount;
	UINT8 CurTrk;
	UINT16 CurPos;
	UINT16 TrkOfs[0x10];
	UINT16 TempOfs;
	
	FileLen = SmpsCfg->SeqLength;
	FileData = SmpsCfg->SeqData;
	CmdList = &SmpsCfg->CmdList;
	CmdMetaList = &SmpsCfg->CmdMetaList;
	DACDrv = &SmpsCfg->DACDrv;
	
	CurPos = 0x00;
	InsPtr = ReadPtr(&FileData[CurPos + 0x00], SmpsCfg);
	FMTrkCnt = FileData[CurPos + 0x02];
	PSGTrkCnt = FileData[CurPos + 0x03];
	
	CurPos += 0x06;
	TempOfs = CurPos + FMTrkCnt * 0x04 + PSGTrkCnt * 0x06;
	
	TrkCount = 0x00;
	for (CurTrk = 0x00; CurTrk < FMTrkCnt; CurTrk ++, TrkCount ++, CurPos += 0x04)
		TrkOfs[TrkCount] = ReadPtr(&FileData[CurPos], SmpsCfg);
	for (CurTrk = 0x00; CurTrk < PSGTrkCnt; CurTrk ++, TrkCount ++, CurPos += 0x06)
		TrkOfs[TrkCount] = ReadPtr(&FileData[CurPos], SmpsCfg);
	for (CurTrk = 0x00; CurTrk < SmpsCfg->AddChnCnt; CurTrk ++, TrkCount ++, CurPos += 0x04)
		TrkOfs[TrkCount] = ReadPtr(&FileData[CurPos], SmpsCfg);
	
	if (InsPtr < FileLen)
	{
		INS_LIB* InsLib;
		
		InsLib = (INS_LIB*)malloc(sizeof(INS_LIB));
		SmpsCfg->InsLib = InsLib;
		InsLib->InsCount = (FileLen - InsPtr) / SmpsCfg->InsRegCnt;
		if (InsLib->InsCount > 0x100)
			InsLib->InsCount = 0x100;
		InsLib->InsPtrs = (UINT8**)malloc(InsLib->InsCount * sizeof(UINT8*));
		for (CurTrk = 0; CurTrk < InsLib->InsCount; CurTrk ++)
			InsLib->InsPtrs[CurTrk] = &FileData[InsPtr + CurTrk * SmpsCfg->InsRegCnt];
	}
	else if (SmpsCfg->GlbInsData != NULL)
	{
		InsPtr += SmpsCfg->SeqBase;
		if (InsPtr > SmpsCfg->GlbInsBase && InsPtr < SmpsCfg->GlbInsBase + SmpsCfg->GlbInsLen)
		{
			INS_LIB* InsLib;
			UINT16 BaseOfs;
			
			BaseOfs = InsPtr - SmpsCfg->GlbInsBase;
			InsLib = (INS_LIB*)malloc(sizeof(INS_LIB));
			SmpsCfg->InsLib = InsLib;
			InsLib->InsCount = (SmpsCfg->GlbInsLen - BaseOfs) / SmpsCfg->InsRegCnt;
			if (InsLib->InsCount > 0x100)
				InsLib->InsCount = 0x100;
			InsLib->InsPtrs = (UINT8**)malloc(InsLib->InsCount * sizeof(UINT8*));
			for (CurTrk = 0; CurTrk < InsLib->InsCount; CurTrk ++)
				InsLib->InsPtrs[CurTrk] = &SmpsCfg->GlbInsData[BaseOfs + CurTrk * SmpsCfg->InsRegCnt];
		}
		else
		{
			SmpsCfg->InsLib = &SmpsCfg->GlbInsLib;
		}
	}
	else
	{
		SmpsCfg->InsLib = NULL;
	}
}

public:
	SMPSInterfaceClass()
	{
		gameWindow = NULL;
	}

	BOOL initialize(HWND hwnd)
	{
		gameWindow = hwnd;
		ZeroMemory(&smpscfg, sizeof(smpscfg));
		s3mode = false;

		smpscfg.PtrFmt = PTRFMT_Z80;
		smpscfg.TempoMode = TEMPO_OVERFLOW;
		smpscfg.FMBaseNote = FMBASEN_C;
		smpscfg.DelayFreq = DLYFREQ_KEEP;
		smpscfg.FM6DACOff = 0x01;       // improve Special Stage -> Chaos Emerald song change
		smpscfg.ModAlgo = MODULAT_Z80;
		smpscfg.EnvMult = ENVMULT_Z80;
		smpscfg.VolMode = VOLMODE_BIT7;

		smpscfg.FMChnCnt = LengthOfArray(FMCHN_ORDER);
		memcpy(smpscfg.FMChnList, FMCHN_ORDER, 7);
		LoadRegisterList(&smpscfg);

		smpscfg.FMFreqs = (UINT16*)DEF_FMFREQ_VAL;
		smpscfg.FMFreqCnt = LengthOfArray(DEF_FMFREQ_VAL);
		smpscfg.PSGFreqs = (UINT16*)DEF_PSGFREQ_Z80_T2_VAL;
		smpscfg.PSGFreqCnt = LengthOfArray(DEF_PSGFREQ_Z80_T2_VAL);
		smpscfg.FM3Freqs = (UINT16*)FM3FREQS;
		smpscfg.FM3FreqCnt = LengthOfArray(FM3FREQS);

		smpscfg.EnvCmds[0] = ENVCMD_RESET;
		smpscfg.EnvCmds[1] = ENVCMD_HOLD;
		smpscfg.EnvCmds[2] = ENVCMD_LOOP;
		smpscfg.EnvCmds[3] = ENVCMD_STOP;
		smpscfg.EnvCmds[4] = ENVCMD_CHGMULT;

		smpscfg.CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS;
		smpscfg.CmdList.FlagBase = 0xE0;
		smpscfg.CmdList.FlagCount = LengthOfArray(CMDFLAGS);

		smpscfg.CmdMetaList.CmdData = (CMD_FLAGS*)CMDMETAFLAGS;
		smpscfg.CmdMetaList.FlagBase = 0;
		smpscfg.CmdMetaList.FlagCount = LengthOfArray(CMDMETAFLAGS);

		ZeroMemory(&smpscfg.DrumLib, sizeof(smpscfg.DrumLib));
		smpscfg.DrumLib.DrumCount = 0x60;
		smpscfg.DrumLib.DrumData = new DRUM_DATA[smpscfg.DrumLib.DrumCount];
		ZeroMemory(smpscfg.DrumLib.DrumData, sizeof(DRUM_DATA) * smpscfg.DrumLib.DrumCount);

		smpscfg.DrumLib.DrumData[0].Type = 0xFF;
		for (int i = 0; i < 95; i++)
			smpscfg.DrumLib.DrumData[i + 1].DrumID = i;

		HRSRC hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_MODULAT), L"MISC");
		std::string s((const char *)LockResource(LoadResource(moduleHandle, hres)), SizeofResource(moduleHandle, hres));
		std::istringstream str(s);
		GetEnvelopeData(str, &smpscfg.ModEnvs);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG), L"MISC");
		s = std::string((const char *)LockResource(LoadResource(moduleHandle, hres)), SizeofResource(moduleHandle, hres));
		str = std::istringstream(s);
		GetEnvelopeData(str, &smpscfg.VolEnvs);

		ZeroMemory(&smpscfg.DACDrv, sizeof(smpscfg.DACDrv));
		smpscfg.DACDrv.SmplAlloc = (IDR_DAC_B2_S3 - IDR_DAC_81);
		smpscfg.DACDrv.SmplCount = (IDR_DAC_B2_S3 - IDR_DAC_81);
		smpscfg.DACDrv.Smpls = new DAC_SAMPLE[smpscfg.DACDrv.SmplAlloc];
		ZeroMemory(smpscfg.DACDrv.Smpls, sizeof(DAC_SAMPLE) * smpscfg.DACDrv.SmplAlloc);

		for (int i = 0; i < (IDR_DAC_B2_S3 - IDR_DAC_81); i++)
		{
			smpscfg.DACDrv.Smpls[i].Compr = COMPR_DPCM;
			smpscfg.DACDrv.Smpls[i].DPCMArr = (UINT8*)DefDPCMData;
			HRSRC hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_81 + i), L"DAC");
			smpscfg.DACDrv.Smpls[i].Size = SizeofResource(moduleHandle, hres);
			smpscfg.DACDrv.Smpls[i].Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		}

		smpscfg.DACDrv.TblAlloc = LengthOfArray(DACFiles);
		smpscfg.DACDrv.TblCount = LengthOfArray(DACFiles);
		smpscfg.DACDrv.SmplTbl = new DAC_TABLE[smpscfg.DACDrv.TblAlloc];
		ZeroMemory(smpscfg.DACDrv.SmplTbl, smpscfg.DACDrv.TblAlloc * sizeof(DAC_TABLE));

		smpscfg.DACDrv.Cfg.BaseRate = 275350;
		smpscfg.DACDrv.Cfg.Divider = (UINT)(10.42 * 100 + 0.5);
		smpscfg.DACDrv.Cfg.Channels = 1;
		smpscfg.DACDrv.Cfg.VolDiv = 1;

		smpscfg.DACDrv.Cfg.BaseCycles = 297;
		smpscfg.DACDrv.Cfg.LoopCycles = 26;
		smpscfg.DACDrv.Cfg.LoopSamples = 2;

		for (size_t i = 0; i < LengthOfArray(DACFiles); i++)
		{
			smpscfg.DACDrv.SmplTbl[i].Sample = DACFiles[i].resid - IDR_DAC_81;
			smpscfg.DACDrv.SmplTbl[i].Rate = DACFiles[i].rate;
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_INSSET), L"MISC");
		smpscfg.GlbInsLen = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg.GlbInsData = (UINT8 *)LockResource(LoadResource(moduleHandle, hres));

		UINT16 InsCount = smpscfg.GlbInsLen / smpscfg.InsRegCnt;
		smpscfg.GlbInsLib.InsCount = InsCount;
		smpscfg.GlbInsLib.InsPtrs = NULL;
		if (InsCount)
		{
			smpscfg.GlbInsLib.InsPtrs = new UINT8*[InsCount];
			auto CurPos = 0x0000;
			for (auto CurIns = 0x00; CurIns < InsCount; CurIns ++, CurPos += smpscfg.InsRegCnt)
				smpscfg.GlbInsLib.InsPtrs[CurIns] = &smpscfg.GlbInsData[CurPos];
		}

		smpscfg.GlbInsBase = 0x17D8;

		DAC_Reset();
		StartAudioOutput();
		InitDriver();

		timeBeginPeriod(2);

		return TRUE;
	}

	BOOL load_song(unsigned char id, unsigned int bgmmode)
	{
		HRSRC hres;
		ThreadPauseConfrm = false;
		PauseThread = true;
		while(! ThreadPauseConfrm)
			Sleep(1);
		id--;
		if (id == (IDR_MUSIC_25 - IDR_MUSIC_1) && Sonic3Mode)
			id = IDR_MUSIC_45 - IDR_MUSIC_1; // Restore S3 Midboss music
		musicentry *song = &MusicFiles[id];
		if (song->s3 & !s3mode) // switch s&k -> s3
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_B2_S3), L"DAC");
			DAC_SAMPLE *samp = &smpscfg.DACDrv.Smpls[IDR_DAC_B2 - IDR_DAC_81];
			samp->Size = SizeofResource(moduleHandle, hres);
			samp->Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));

			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG_S3), L"MISC");
			std::string s((const char *)LockResource(LoadResource(moduleHandle, hres)), SizeofResource(moduleHandle, hres));
			std::istringstream str(s);
			GetEnvelopeData(str, &smpscfg.VolEnvs);
		}
		else if (!song->s3 & s3mode) // switch s3 -> s&k
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_B2), L"DAC");
			DAC_SAMPLE *samp = &smpscfg.DACDrv.Smpls[IDR_DAC_B2 - IDR_DAC_81];
			samp->Size = SizeofResource(moduleHandle, hres);
			samp->Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));

			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG), L"MISC");
			std::string s((const char *)LockResource(LoadResource(moduleHandle, hres)), SizeofResource(moduleHandle, hres));
			std::istringstream str(s);
			GetEnvelopeData(str, &smpscfg.VolEnvs);
		}
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MUSIC_1 + id), L"MUSIC");
		smpscfg.SeqBase = song->base;
		smpscfg.SeqLength = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg.SeqData = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		PreparseSMPSFile(&smpscfg);
		return TRUE;
	}

	BOOL play_song()
	{
		ThreadPauseConfrm = false;
		PauseThread = true;
		while(! ThreadPauseConfrm)
			Sleep(1);
		PauseStream(false);
		FrameDivider = 60;
		PlayMusic(&smpscfg);
		PauseThread = false;
		return TRUE;
	}

	BOOL stop_song()
	{
		ThreadPauseConfrm = false;
		PauseThread = true;
		while(! ThreadPauseConfrm)
			Sleep(1);
		StopAllSound();
		return TRUE;
	}

	BOOL pause_song()
	{
		PauseStream(true);
		return TRUE;
	}

	BOOL unpause_song()
	{
		PauseStream(false);
		return TRUE;
	}

	BOOL set_song_tempo(unsigned int pct)
	{
		FrameDivider = (UINT32)(1 / (pct / 100.0) * 60);
		return TRUE;
	}

	~SMPSInterfaceClass()
	{
		StopAllSound();
		CloseThread = true;
		timeEndPeriod(2);
	}
};

SMPSInterfaceClass midiInterface;

extern "C" __declspec(dllexport) SMPSInterfaceClass *GetMidiInterface()
{
	return &midiInterface;
}

void NotifySongStopped()
{
	PostMessageA(gameWindow, 0x464u, 0, 0);
}