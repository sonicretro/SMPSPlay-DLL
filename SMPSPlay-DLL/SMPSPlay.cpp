// SMPSPlay.cpp : Defines the exported functions for the DLL application.
//

#ifdef _MSC_VER
#include <tchar.h>
#else
#define _T(x) L ## x
#endif
#include <windows.h>

extern "C"
{
	// defines required for SMPSPlay files:
	//  DISABLE_DLOAD_FILE
	//  DISABLE_DEBUG_MSGS
	//  DISABLE_NECPCM
#include <common_def.h>
#include "../SMPSPlay/src/Engine/smps.h"
#include "../SMPSPlay/src/Engine/smps_commands.h"
#include "../SMPSPlay/src/Sound.h"
#include "../SMPSPlay/src/loader_data.h"
#include "../SMPSPlay/src/loader_smps.h"
}

#include "SMPSPlay.h"
#include <sstream>
#if ! defined(_MSC_VER) || _MSC_VER >= 1600
#include "IniFile.hpp"
#define INISUPPORT 1
#endif
#include <fstream>
#include <vector>
#include <ctime>
#include "resource.h"
#include "resource.gen.h"
#include "songinfo.h"
#include "musicid.gen.h"
using namespace std;

#if defined(_MSC_VER) && _MSC_VER >= 1400
#define get_time_int(x)	_time32(x)	// use _time32 for VC2005 and later
#else
#define get_time_int(x)	time(x)
#endif


HMODULE moduleHandle;
extern "C"
{
	//extern UINT32 SampleRate;	// from Sound.c
	extern UINT16 FrameDivider;
	extern INT32 OutputVolume;
	extern AUDIO_CFG AudioCfg;

	static void NotifySongStopped(void);
}

enum MusicID2 {
	MusicID_ByCharacter = -5,
	MusicID_ByZone,
	MusicID_MIDI,
	MusicID_Random,
	MusicID_Default,
	MusicID_Midboss = MusicID_SKMidboss,
	MusicID_Unused = MusicID_S3Midboss,
	MusicID_HiddenPalace = MusicID_SKCredits + 1,
	MusicID_SuperSonic,
	MusicID_Ending,
	MusicID_DataSelect,
	MusicID_SpecialStageResult,
	MusicID_BlueSphereResult,
	MusicID_BlueSphereTitle,
	MusicID_BlueSphereDifficulty,
	MusicID_TimeAttackRecords,
	TrackCount
};

struct dacentry { int resid; unsigned char rate; };

static dacentry DACFiles[] = {
	{ IDR_DAC_81, 0x04 },
	{ IDR_DAC_82, 0x0E },
	{ IDR_DAC_82, 0x14 },
	{ IDR_DAC_82, 0x1A },
	{ IDR_DAC_82, 0x20 },
	{ IDR_DAC_86, 0x04 },
	{ IDR_DAC_87, 0x04 },
	{ IDR_DAC_88, 0x06 },
	{ IDR_DAC_89, 0x0A },
	{ IDR_DAC_8A, 0x14 },
	{ IDR_DAC_8A, 0x1B },
	{ IDR_DAC_8C, 0x08 },
	{ IDR_DAC_8D, 0x0B },
	{ IDR_DAC_8D, 0x11 },
	{ IDR_DAC_8F, 0x08 },
	{ IDR_DAC_90, 0x03 },
	{ IDR_DAC_90, 0x07 },
	{ IDR_DAC_90, 0x0A },
	{ IDR_DAC_90, 0x0E },
	{ IDR_DAC_94, 0x06 },
	{ IDR_DAC_94, 0x0A },
	{ IDR_DAC_94, 0x0D },
	{ IDR_DAC_94, 0x12 },
	{ IDR_DAC_98, 0x0B },
	{ IDR_DAC_98, 0x13 },
	{ IDR_DAC_98, 0x16 },
	{ IDR_DAC_9B, 0x0C },
	{ IDR_DAC_9C, 0x0A },
	{ IDR_DAC_9D, 0x18 },
	{ IDR_DAC_9E, 0x18 },
	{ IDR_DAC_9F, 0x0C },
	{ IDR_DAC_A0, 0x0C },
	{ IDR_DAC_A1, 0x0A },
	{ IDR_DAC_A2, 0x0A },
	{ IDR_DAC_A3, 0x18 },
	{ IDR_DAC_A4, 0x18 },
	{ IDR_DAC_A5, 0x0C },
	{ IDR_DAC_A6, 0x09 },
	{ IDR_DAC_A7, 0x18 },
	{ IDR_DAC_A8, 0x18 },
	{ IDR_DAC_A9, 0x0C },
	{ IDR_DAC_AA, 0x0A },
	{ IDR_DAC_AB, 0x0D },
	{ IDR_DAC_AC, 0x06 },
	{ IDR_DAC_AD, 0x10 },
	{ IDR_DAC_AD, 0x18 },
	{ IDR_DAC_AF, 0x09 },
	{ IDR_DAC_AF, 0x12 },
	{ IDR_DAC_B1, 0x18 },
	{ IDR_DAC_B2, 0x16 },
	{ IDR_DAC_B2, 0x20 },
	{ IDR_DAC_B4, 0x0C },
	{ IDR_DAC_B5, 0x0C },
	{ IDR_DAC_B6, 0x0C },
	{ IDR_DAC_B7, 0x18 },
	{ IDR_DAC_B8, 0x0C },
	{ IDR_DAC_B8, 0x0C },
	{ IDR_DAC_BA, 0x18 },
	{ IDR_DAC_BB, 0x18 },
	{ IDR_DAC_BC, 0x18 },
	{ IDR_DAC_BD, 0x0C },
	{ IDR_DAC_BE, 0x0C },
	{ IDR_DAC_BF, 0x1C },
	{ IDR_DAC_C0, 0x0B },
	{ IDR_DAC_B4, 0x0F },
	{ IDR_DAC_B4, 0x11 },
	{ IDR_DAC_B4, 0x12 },
	{ IDR_DAC_B4, 0x0B },	// 0xC4 [last S3K DAC ID]
	{ IDR_DAC_B2_S3, 0x16 },
	{ IDR_DAC_B2_S3, 0x20 },
	{ IDR_DAC_9B_S3PROTO, 0x12 },
	{ IDR_DAC_9F_S3D, 0x01 },
	{ IDR_DAC_A0_S3D, 0x12 },
};
#define S3_DACB2_ID_BASE	(0xC5 - 0x81)
#define S3P_DAC9B_ID_BASE	(0xC7 - 0x81)
#define S3D_DAC9F_ID_BASE	(0xC8 - 0x81)

static UINT8 FMDrumList[] = {
	// 0    1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	      0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x86, 0x87, 0x82, 0x83, 0x82, 0x84, 0x82, 0x85,	// 81..8F
	0x82, 0x83, 0x84, 0x85, 0x82, 0x83, 0x84, 0x85, 0x82, 0x83, 0x84, 0x81, 0x88, 0x86, 0x81, 0x81,	// 90..9F
	0x87, 0x87, 0x87, 0x81, 0x86, 0x00, 0x00, 0x86, 0x86, 0x87, 0x87, 0x81, 0x85, 0x82, 0x83, 0x82,	// A0..AF
	0x84, 0x87, 0x82, 0x84, 0x00, 0x00, 0x87, 0x86, 0x86, 0x86, 0x87, 0x87, 0x81, 0x86, 0x00, 0x81,	// B0..BF
	0x86, 0x00, 0x00, 0x00, 0x00                                                                   	// C0..C4
};

static dacentry DACFiles_S2[] = {
	{ IDR_DAC_81_S2, 0x17 },
	{ IDR_DAC_82_S2, 0x01 },
	{ IDR_DAC_83_S2, 0x06 },
	{ IDR_DAC_84_S2, 0x08 },
	{ IDR_DAC_85_S2, 0x1B },
	{ IDR_DAC_86_S2, 0x0A },
	{ IDR_DAC_87_S2, 0x1B }
};

static UINT8 S2DrumList[][2] = {
	{0x00, 0x00},
	{0x81, 0x17},
	{0x82, 0x01},
	{0x83, 0x06},
	{0x84, 0x08},
	{0x85, 0x1B},
	{0x86, 0x0A},
	{0x87, 0x1B},
	{0x85, 0x12},
	{0x85, 0x15},
	{0x85, 0x1C},
	{0x85, 0x1D},
	{0x86, 0x02},
	{0x86, 0x05},
	{0x86, 0x08},
	{0x87, 0x08},
	{0x97, 0x0B},
	{0x97, 0x12},
};

static const UINT8 DefDPCMData[] = {
	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
	0x80, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0 };

static const UINT8 FMCHN_ORDER[] = { 0x16, 0, 1, 2, 4, 5, 6 };
static const UINT8 PSGCHN_ORDER[] = { 0x80, 0xA0, 0xC0 };

static const UINT16 DEF_FMFREQ_VAL[13] =
{ 0x25E, 0x284, 0x2AB, 0x2D3, 0x2FE, 0x32D, 0x35C, 0x38F, 0x3C5, 0x3FF, 0x43C, 0x47C, 0x4C0 };

static const UINT16 DEF_PSGFREQ_68K_VAL[] = {
	0x356, 0x326, 0x2F9, 0x2CE, 0x2A5, 0x280, 0x25C, 0x23A, 0x21A, 0x1FB, 0x1DF, 0x1C4,
	0x1AB, 0x193, 0x17D, 0x167, 0x153, 0x140, 0x12E, 0x11D, 0x10D, 0x0FE, 0x0EF, 0x0E2,
	0x0D6, 0x0C9, 0x0BE, 0x0B4, 0x0A9, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071,
	0x06B, 0x065, 0x05F, 0x05A, 0x055, 0x050, 0x04B, 0x047, 0x043, 0x040, 0x03C, 0x039,
	0x036, 0x033, 0x030, 0x02D, 0x02B, 0x028, 0x026, 0x024, 0x022, 0x020, 0x01F, 0x01D,
	0x01B, 0x01A, 0x018, 0x017, 0x016, 0x015, 0x013, 0x012, 0x011, 0x000
};

static const UINT16 DEF_PSGFREQ_Z80_T2_VAL[] = {
	0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3F7, 0x3BE, 0x388,
	0x356, 0x326, 0x2F9, 0x2CE, 0x2A5, 0x280, 0x25C, 0x23A, 0x21A, 0x1FB, 0x1DF, 0x1C4,
	0x1AB, 0x193, 0x17D, 0x167, 0x153, 0x140, 0x12E, 0x11D, 0x10D, 0x0FE, 0x0EF, 0x0E2,
	0x0D6, 0x0C9, 0x0BE, 0x0B4, 0x0A9, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071,
	0x06B, 0x065, 0x05F, 0x05A, 0x055, 0x050, 0x04B, 0x047, 0x043, 0x040, 0x03C, 0x039,
	0x036, 0x033, 0x030, 0x02D, 0x02B, 0x028, 0x026, 0x024, 0x022, 0x020, 0x01F, 0x01D,
	0x01B, 0x01A, 0x018, 0x017, 0x016, 0x015, 0x013, 0x012, 0x011, 0x010, 0x000, 0x000
};

static const UINT16 FM3FREQS[] = { 0, 0x132, 0x18E, 0x1E4, 0x234, 0x27E, 0x2C2, 0x2F0 };

static const CMD_FLAGS CMDFLAGS_S12[] = {
	{ CF_PANAFMS, CFS_PAFMS_PAN, 0x02 },
	{ CF_DETUNE, 0x00, 0x02 },
	{ CF_SET_COMM, 0x00, 0x02 },
	{ CF_RETURN, 0x00, 0x01 },
	{ CF_FADE_IN_SONG, 0x00, 0x01 },
	{ CF_TICK_MULT, CFS_TMULT_CUR, 0x02 },
	{ CF_VOLUME, CFS_VOL_NN_FM, 0x02 },
	{ CF_HOLD, 0x00, 0x01 },
	{ CF_NOTE_STOP, CFS_NSTOP_NORMAL, 0x02 },
	{ CF_TRANSPOSE, CFS_TRNSP_ADD, 0x02 },
	{ CF_TEMPO, CFS_TEMPO_SET, 0x02 },
	{ CF_TICK_MULT, CFS_TMULT_ALL, 0x02 },
	{ CF_VOLUME, CFS_VOL_NN_PSG, 0x02 },
	{ CF_IGNORE, 0x00, 0x01 },	// Clear Pushing Flag
	{ CF_IGNORE, 0x00, 0x01 },
	{ CF_INSTRUMENT, CFS_INS_N_FM, 0x82 },
	{ CF_MOD_SETUP, 0x00, 0x05 },
	{ CF_MOD_SET, CFS_MODS_ON, 0x01 },
	{ CF_TRK_END, CFS_TEND_STD, 0x01 },
	{ CF_PSG_NOISE, CFS_PNOIS_SET, 0x02 },
	{ CF_MOD_SET, CFS_MODS_OFF, 0x01 },
	{ CF_INSTRUMENT, CFS_INS_C_PSG, 0x02 },
	{ CF_GOTO, 0x00, 0x03, 0x01 },
	{ CF_LOOP, 0x00, 0x05, 0x03 },
	{ CF_GOSUB, 0x00, 0x03, 0x01 },
	{ CF_SND_OFF, 0x00, 0x01, 0x01 }
};

static const CMD_FLAGS CMDFLAGS_S3K[] = {
	{ CF_PANAFMS, CFS_PAFMS_PAN, 0x02 },
	{ CF_DETUNE, 0x00, 0x02 },
	{ CF_FADE_IN_SONG, 0x00, 0x02 },
	{ CF_TRK_END, CFS_TEND_MUTE, 0x01 },
	{ CF_VOLUME, CFS_VOL_ABS_S3K, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_FMP2, 0x03 },
	{ CF_VOLUME, CFS_VOL_CC_FM, 0x02 },
	{ CF_HOLD, 0x00, 0x01 },
	{ CF_NOTE_STOP, CFS_NSTOP_MULT, 0x02 },
	{ CF_SPINDASH_REV, CFS_SDREV_INC, 0x01 },
	{ CF_PLAY_DAC, 0x00, 0x02 },
	{ CF_LOOP_EXIT, 0x00, 0x04, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_PSG, 0x02 },
	{ CF_TRANSPOSE, CFS_TRNSP_SET_S3K, 0x02 },
	{ CF_FM_COMMAND, 0x00, 0x03 },
	{ CF_INSTRUMENT, CFS_INS_N_FM, 0x82 },
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
	{ CF_CONT_SFX, 0x00, 0x03, 0x01 },
	{ CF_RAW_FREQ, 0x00, 0x02 },
	{ CF_SPC_FM3, 0x00, 0x05 },
	{ CF_META_CF, 0x00, 0x01 }
};

static const CMD_FLAGS CMDMETAFLAGS_S3K[] = {
	{ CF_TEMPO, CFS_TEMPO_SET, 0x02 },
	{ CF_SND_CMD, 0x00, 0x02 },
	{ CF_MUS_PAUSE, CFS_MUSP_Z80, 0x02 },
	{ CF_COPY_MEM, 0x00, 0x03 },
	{ CF_TICK_MULT, CFS_TMULT_ALL, 0x02 },
	{ CF_SSG_EG, CFS_SEG_NORMAL, 0x05 },
	{ CF_FM_VOLENV, 0x00, 0x03 },
	{ CF_SPINDASH_REV, CFS_SDREV_RESET, 0x01 }
};

static const CMD_FLAGS CMDFLAGS_S3PROTO[] = {
	{ CF_PANAFMS, CFS_PAFMS_PAN, 0x02 },
	{ CF_DETUNE, 0x00, 0x02 },
	{ CF_FADE_IN_SONG, 0x00, 0x02 },
	{ CF_TRK_END, CFS_TEND_MUTE, 0x01 },
	{ CF_VOLUME, CFS_VOL_ABS_S3K, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_FMP2, 0x03 },
	{ CF_VOLUME, CFS_VOL_CC_FM, 0x02 },
	{ CF_HOLD, 0x00, 0x01 },
	{ CF_NOTE_STOP, CFS_NSTOP_MULT, 0x02 },
	{ CF_SPINDASH_REV, CFS_SDREV_INC, 0x01 },	// not used by music (and completely broken), so keep S3 final command
	{ CF_PLAY_DAC, 0x00, 0x02 },
	{ CF_LOOP_EXIT, 0x00, 0x04, 0x02 },
	{ CF_VOLUME, CFS_VOL_CC_PSG, 0x02 },
	{ CF_TRANSPOSE, CFS_TRNSP_SET_S3K, 0x02 },
	{ CF_FM_COMMAND, 0x00, 0x03 },
	{ CF_INSTRUMENT, CFS_INS_N_FM, 0x82 },
	{ CF_MOD_SETUP, 0x00, 0x05 },
	{ CF_MOD_ENV, CFS_MENV_1FMP, 0x03 },	// different from S3 final
	{ CF_TRK_END, CFS_TEND_STD, 0x01 },
	{ CF_PSG_NOISE, CFS_PNOIS_SRES, 0x02 },
	{ CF_MOD_ENV, CFS_MENV_1GEN, 0x02 },	// different from S3 final
	{ CF_INSTRUMENT, CFS_INS_C_PSG, 0x02 },
	{ CF_GOTO, 0x00, 0x03, 0x01 },
	{ CF_LOOP, 0x00, 0x05, 0x03 },
	{ CF_GOSUB, 0x00, 0x03, 0x01 },
	{ CF_RETURN, 0x00, 0x01 },
	{ CF_MOD_SET, CFS_MODS_OFF, 0x01 },
	{ CF_TRANSPOSE, CFS_TRNSP_ADD, 0x02 },
	{ CF_CONT_SFX, 0x00, 0x03, 0x01 },
	{ CF_RAW_FREQ, 0x00, 0x02 },
	{ CF_SPC_FM3, 0x00, 0x05 },
	{ CF_META_CF, 0x00, 0x01 }
};

static const UINT8 INSOPS_DEFAULT[] =
{ 0xB0,
0x30, 0x38, 0x34, 0x3C,
0x50, 0x58, 0x54, 0x5C,
0x60, 0x68, 0x64, 0x6C,
0x70, 0x78, 0x74, 0x7C,
0x80, 0x88, 0x84, 0x8C,
0x40, 0x48, 0x44, 0x4C,
0x00 };	// The 0 is required by the SMPS routines to terminate the array.

static const UINT8 INSOPS_HARDWARE[] =
{ 0xB0,
0x30, 0x34, 0x38, 0x3C,
0x50, 0x54, 0x58, 0x5C,
0x60, 0x64, 0x68, 0x6C,
0x70, 0x74, 0x78, 0x7C,
0x80, 0x84, 0x88, 0x8C,
0x40, 0x44, 0x48, 0x4C,
0x00 };	// The 0 is required by the SMPS routines to terminate the array.

enum DriverMode {
	SmpsDrv_S3K,
	SmpsDrv_S12
};

struct SongInfo { UINT8 *data; UINT16 length; UINT16 base; unsigned char mode; };

static vector<SongInfo> songs(SongCount);

static vector<const char *> songnames;

static UINT8* SmpsReloadState;
static UINT8* SmpsMusicSaveState;
static bool default1UpHandling = true;

static SMPS_CFG smpscfg_3K;
static SMPS_CFG smpscfg_12;
static SMPS_CFG* cursmpscfg;
static SMPS_SET cursmps;
static ENV_LIB ModEnvs_S3Proto;
static ENV_LIB ModEnvs_S3K;
static ENV_LIB VolEnvs_S3;
static ENV_LIB VolEnvs_SK;
static bool fmdrum_on;

INLINE UINT16 ReadBE16(const UINT8* Data)
{
	return (Data[0x00] << 8) | (Data[0x01] << 0);
}

INLINE UINT16 ReadLE16(const UINT8* Data)
{
	return (Data[0x01] << 8) | (Data[0x00] << 0);
}

static void LoadRegisterList(SMPS_CFG* SmpsCfg, UINT8 RegCnt, const UINT8* RegPtr)
{
	UINT8 CurReg;
	UINT8 RegTL_Idx;

	RegTL_Idx = 0xFF;
	for (CurReg = 0x00; CurReg < RegCnt; CurReg++)
	{
		if (RegPtr[CurReg] == 0x00 || (RegPtr[CurReg] & 0x03))
			break;
		if ((RegPtr[CurReg] & 0xF0) == 0x40 && RegTL_Idx == 0xFF)
			RegTL_Idx = CurReg;
	}
	RegCnt = CurReg;
	if (!RegCnt)
	{
		SmpsCfg->InsRegCnt = 0x00;
		SmpsCfg->InsRegs = NULL;
		SmpsCfg->InsReg_TL = NULL;
		return;
	}

	SmpsCfg->InsRegCnt = RegCnt;
	SmpsCfg->InsRegs = (UINT8*)RegPtr;
	if (RegTL_Idx == 0xFF)
		SmpsCfg->InsReg_TL = NULL;
	else
		SmpsCfg->InsReg_TL = (UINT8*)&RegPtr[RegTL_Idx];

	return;
}

#if ! defined(_MSC_VER) || _MSC_VER >= 1600
template <typename T, size_t N>
static inline size_t LengthOfArray(const T(&)[N])
{
	return N;
}
#else
#define LengthOfArray(x)	(sizeof(x) / sizeof((x)[0]))
#endif

static void LoadSettings(UINT8 Mode, SMPS_CFG* SmpsCfg)
{
	switch (Mode)
	{
	case SmpsDrv_S12:
		SmpsCfg->PtrFmt = PTRFMT_Z80;

		SmpsCfg->InsMode = INSMODE_DEF;
		LoadRegisterList(SmpsCfg, (UINT8)LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
		SmpsCfg->FMChnCnt = (UINT8)LengthOfArray(FMCHN_ORDER);
		memcpy(SmpsCfg->FMChnList, FMCHN_ORDER, SmpsCfg->FMChnCnt);
		SmpsCfg->PSGChnCnt = (UINT8)LengthOfArray(PSGCHN_ORDER);
		memcpy(SmpsCfg->PSGChnList, PSGCHN_ORDER, SmpsCfg->PSGChnCnt);
		SmpsCfg->AddChnCnt = 0;

		SmpsCfg->TempoMode = TEMPO_TIMEOUT;
		SmpsCfg->Tempo1Tick = T1TICK_DOTEMPO;	// S1/S2B: T1TICK_DOTEMPO, S2F: T1TICK_NOTEMPO
		SmpsCfg->FMBaseNote = FMBASEN_B;
		SmpsCfg->FMBaseOct = 0;
		SmpsCfg->FMOctWrap = 0;
		SmpsCfg->NoteOnPrevent = NONPREV_REST;
		SmpsCfg->DelayFreq = DLYFREQ_RESET;
		SmpsCfg->FM6DACOff = 0x01;	// improve Special Stage -> Chaos Emerald song change
		SmpsCfg->ModAlgo = MODULAT_68K;
		SmpsCfg->EnvMult = ENVMULT_68K;
		SmpsCfg->VolMode = VOLMODE_ALGO;
		SmpsCfg->DrumChnMode = DCHNMODE_NORMAL;

		SmpsCfg->FMFreqs = (UINT16*)&DEF_FMFREQ_VAL[0];
		SmpsCfg->FMFreqCnt = 12;
		SmpsCfg->PSGFreqs = (UINT16*)DEF_PSGFREQ_68K_VAL;
		SmpsCfg->PSGFreqCnt = (UINT8)LengthOfArray(DEF_PSGFREQ_68K_VAL);
		SmpsCfg->FM3Freqs = (UINT16*)FM3FREQS;
		SmpsCfg->FM3FreqCnt = (UINT8)LengthOfArray(FM3FREQS);

		SmpsCfg->FadeMode = FADEMODE_68K;
		SmpsCfg->FadeOut.Steps = 0x28;
		SmpsCfg->FadeOut.Delay = 3;
		SmpsCfg->FadeOut.AddFM = 1;
		SmpsCfg->FadeOut.AddPSG = 1;
		SmpsCfg->FadeIn.Steps = 0x28;
		SmpsCfg->FadeIn.Delay = 2;
		SmpsCfg->FadeIn.AddFM = 1;
		SmpsCfg->FadeIn.AddPSG = 1;

		SmpsCfg->EnvCmds[0] = ENVCMD_HOLD;

		SmpsCfg->NoteBase = 0x80;
		SmpsCfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S12;
		SmpsCfg->CmdList.FlagBase = 0xE0;
		SmpsCfg->CmdList.FlagCount = (UINT16)LengthOfArray(CMDFLAGS_S12);
		SmpsCfg->CmdMetaList.CmdData = NULL;
		SmpsCfg->CmdMetaList.FlagBase = 0x00;
		SmpsCfg->CmdMetaList.FlagCount = 0;
		break;
	case SmpsDrv_S3K:
		SmpsCfg->PtrFmt = PTRFMT_Z80;

		SmpsCfg->InsMode = INSMODE_DEF;
		LoadRegisterList(SmpsCfg, (UINT8)LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
		SmpsCfg->FMChnCnt = (UINT8)LengthOfArray(FMCHN_ORDER);
		memcpy(SmpsCfg->FMChnList, FMCHN_ORDER, SmpsCfg->FMChnCnt);
		SmpsCfg->PSGChnCnt = (UINT8)LengthOfArray(PSGCHN_ORDER);
		memcpy(SmpsCfg->PSGChnList, PSGCHN_ORDER, SmpsCfg->PSGChnCnt);
		SmpsCfg->AddChnCnt = 0;

		SmpsCfg->TempoMode = TEMPO_OVERFLOW;
		SmpsCfg->Tempo1Tick = T1TICK_DOTEMPO;
		SmpsCfg->FMBaseNote = FMBASEN_C;
		SmpsCfg->FMBaseOct = 0;
		SmpsCfg->FMOctWrap = 0;
		SmpsCfg->NoteOnPrevent = NONPREV_HOLD;
		SmpsCfg->DelayFreq = DLYFREQ_KEEP;
		SmpsCfg->FM6DACOff = 0x01;	// improve Special Stage -> Chaos Emerald song change
		SmpsCfg->ModAlgo = MODULAT_Z80;
		SmpsCfg->EnvMult = ENVMULT_Z80;
		SmpsCfg->VolMode = VOLMODE_BIT7;
		SmpsCfg->DrumChnMode = DCHNMODE_NORMAL;

		SmpsCfg->FMFreqs = (UINT16*)&DEF_FMFREQ_VAL[1];
		SmpsCfg->FMFreqCnt = 12;
		SmpsCfg->PSGFreqs = (UINT16*)DEF_PSGFREQ_Z80_T2_VAL;
		SmpsCfg->PSGFreqCnt = (UINT8)LengthOfArray(DEF_PSGFREQ_Z80_T2_VAL);
		SmpsCfg->FM3Freqs = (UINT16*)FM3FREQS;
		SmpsCfg->FM3FreqCnt = (UINT8)LengthOfArray(FM3FREQS);

		SmpsCfg->FadeMode = FADEMODE_Z80;
		SmpsCfg->FadeOut.Steps = 0x28;
		SmpsCfg->FadeOut.Delay = 6;
		SmpsCfg->FadeOut.AddFM = 1;
		SmpsCfg->FadeOut.AddPSG = 1;
		SmpsCfg->FadeIn.Steps = 0x40;
		SmpsCfg->FadeIn.Delay = 2;
		SmpsCfg->FadeIn.AddFM = 1;
		SmpsCfg->FadeIn.AddPSG = 1;

		SmpsCfg->EnvCmds[0] = ENVCMD_RESET;
		SmpsCfg->EnvCmds[1] = ENVCMD_HOLD;
		SmpsCfg->EnvCmds[2] = ENVCMD_LOOP;
		SmpsCfg->EnvCmds[3] = ENVCMD_STOP;
		SmpsCfg->EnvCmds[4] = ENVCMD_CHGMULT;

		SmpsCfg->NoteBase = 0x80;
		SmpsCfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S3K;
		SmpsCfg->CmdList.FlagBase = 0xE0;
		SmpsCfg->CmdList.FlagCount = (UINT16)LengthOfArray(CMDFLAGS_S3K);
		SmpsCfg->CmdMetaList.CmdData = (CMD_FLAGS*)CMDMETAFLAGS_S3K;
		SmpsCfg->CmdMetaList.FlagBase = 0x00;
		SmpsCfg->CmdMetaList.FlagCount = (UINT16)LengthOfArray(CMDMETAFLAGS_S3K);
		break;
	}
}


static void GenerateDACAlgos(DAC_ALGO* DACAlgo, UINT8 Mode, UINT32 DacCycles)
{
	DACAlgo->BaseCycles = DacCycles;	// BaseCyles overrides BaseRate and Divider
	if (Mode == COMPR_DPCM)
	{
		DACAlgo->LoopCycles = 26;
		DACAlgo->LoopSamples = 2;
	}
	else //if (Mode == COMPR_PCM)
	{
		DACAlgo->LoopCycles = 13;
		DACAlgo->LoopSamples = 1;
	}
	DACAlgo->RateMode = DACRM_DELAY;
	DACAlgo->DefCompr = Mode;
	return;
}

static void GenerateDACDrv(DAC_CFG* DACDrv, UINT16 smplCount, WORD startID, UINT32 dacCycles,
	unsigned int smplTblSize, const dacentry* smplTblList)
{
	unsigned int i;
	DAC_SETTINGS* DACCfg;
	HRSRC hres;

	memset(DACDrv, 0x00, sizeof(DAC_CFG));

	DACDrv->SmplAlloc = smplCount;
	DACDrv->Smpls = new DAC_SAMPLE[DACDrv->SmplAlloc];
	memset(DACDrv->Smpls, 0x00, DACDrv->SmplAlloc * sizeof(DAC_SAMPLE));
	for (i = 0; i < smplCount; i++)
	{
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(startID + i), _T("DAC"));
		DACDrv->Smpls[i].Compr = COMPR_DPCM;
		DACDrv->Smpls[i].DPCMArr = (UINT8*)DefDPCMData;
		DACDrv->Smpls[i].Size = SizeofResource(moduleHandle, hres);
		DACDrv->Smpls[i].Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
	}
	DACDrv->SmplCount = smplCount;

	DACCfg = &DACDrv->Cfg;
	DACCfg->AlgoAlloc = 1;
	DACCfg->Algos = new DAC_ALGO[DACCfg->AlgoAlloc];
	memset(DACCfg->Algos, 0x00, DACCfg->AlgoAlloc * sizeof(DAC_ALGO));
	GenerateDACAlgos(&DACCfg->Algos[0], COMPR_DPCM, 297);
	DACCfg->AlgoCount = 1;

	DACCfg->SmplMode = DACSM_NORMAL;
	DACCfg->Channels = 1;
	DACCfg->VolDiv = 1;

	DACDrv->TblAlloc = (UINT16)smplTblSize;
	DACDrv->SmplTbl = new DAC_TABLE[DACDrv->TblAlloc];
	memset(DACDrv->SmplTbl, 0x00, DACDrv->TblAlloc * sizeof(DAC_TABLE));
	for (i = 0; i < smplTblSize; i++)
	{
		DACDrv->SmplTbl[i].Sample = smplTblList[i].resid - startID;
		DACDrv->SmplTbl[i].Rate = smplTblList[i].rate;
	}
	DACDrv->TblCount = (UINT16)smplTblSize;

	DACDrv->BankCount = 0;
	DACDrv->BankAlloc = 0;
	DACDrv->BankTbl = NULL;

	return;
}

static string GetDirectory(const string& path)
{
	auto slash = path.find_last_of("/\\");
	if (slash == string::npos)
	{
		return string();
	}

	if (slash != path.size() - 1)
	{
		return path.substr(0, slash);
	}

	auto last = slash;
	slash = path.find_last_of("/\\", last);
	if (slash == string::npos)
	{
		return string();
	}

	return path.substr(last);
}

extern "C"
{
	SMPSPlay_API void SMPS_AddCustomSongs(const char *fn)
	{
#ifdef INISUPPORT
		IniFile custsongs(fn);

		string path = GetDirectory(fn);

		for (auto iter = custsongs.begin(); iter != custsongs.end(); iter++)
		{
			if (iter->first.empty()) continue;
			IniGroup *group = iter->second;
			SongInfo si = {};
			string str = group->getString("Type");
			if (str == "S1")
				si.mode = TrackMode_S1;
			else if (str == "S2")
				si.mode = TrackMode_S2;
			else if (str == "S2B")
				si.mode = TrackMode_S2B;
			else if (str == "S3D")
				si.mode = TrackMode_S3D;
			else if (str == "S3")
				si.mode = TrackMode_S3;
			else if (str == "S3P")
				si.mode = TrackMode_S3P;
			si.base = group->getHexInt("Offset");
			FILE *fi;
			str = path + group->getString("File");
			fopen_s(&fi, str.c_str(), "rb");
			if (fi != nullptr)
			{
				fseek(fi, 0, SEEK_END);
				si.length = (UINT16)ftell(fi);
				fseek(fi, 0, SEEK_SET);
				si.data = new UINT8[si.length];
				fread(si.data, 1, si.length, fi);
				fclose(fi);
				songs.push_back(si);
				char *buf = new char[iter->first.length() + 1];
				strncpy_s(buf, iter->first.length() + 1, iter->first.c_str(), iter->first.length());
				buf[iter->first.length()] = 0;
				songnames.push_back(buf);
			}
		}
#endif
	}

	SMPSPlay_API BOOL SMPS_InitializeDriver()
	{
		HRSRC hres;
		UINT8* dataPtr;
		UINT32 dataSize;
		unsigned int i;

		ZeroMemory(&smpscfg_3K, sizeof(smpscfg_3K));
		ZeroMemory(&smpscfg_12, sizeof(smpscfg_12));
		cursmpscfg = NULL;
		fmdrum_on = false;

		for (i = 0; i < SongCount; i++)
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MUSIC_1 + i), _T("MUSIC"));
			songs[i].mode = MusicFiles[i].mode;
			songs[i].base = MusicFiles[i].base;
			songs[i].length = (UINT16)SizeofResource(moduleHandle, hres);
			songs[i].data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
			songnames.push_back(MusicFiles[i].name);
		}

#ifdef INISUPPORT
		SMPS_AddCustomSongs("songs_cust.ini");
#endif

		LoadSettings(SmpsDrv_S3K, &smpscfg_3K);
		LoadSettings(SmpsDrv_S12, &smpscfg_12);

		ZeroMemory(&smpscfg_3K.DrumLib, sizeof(smpscfg_3K.DrumLib));
		smpscfg_3K.DrumLib.DrumCount = 0x60;
		smpscfg_3K.DrumLib.DrumData = new DRUM_DATA[smpscfg_3K.DrumLib.DrumCount];
		ZeroMemory(smpscfg_3K.DrumLib.DrumData, sizeof(DRUM_DATA) * smpscfg_3K.DrumLib.DrumCount);

		smpscfg_3K.DrumLib.DrumData[0].Type = DRMTYPE_NONE;
		for (i = 1; i < smpscfg_3K.DrumLib.DrumCount; i++)
		{
			smpscfg_3K.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
			smpscfg_3K.DrumLib.DrumData[i].DrumID = i - 1;	// set DAC sample
		}

		ZeroMemory(&smpscfg_12.DrumLib, sizeof(smpscfg_12.DrumLib));
		smpscfg_12.DrumLib.DrumCount = (UINT8)LengthOfArray(S2DrumList);
		smpscfg_12.DrumLib.DrumData = new DRUM_DATA[smpscfg_12.DrumLib.DrumCount];
		ZeroMemory(smpscfg_12.DrumLib.DrumData, sizeof(DRUM_DATA) * smpscfg_12.DrumLib.DrumCount);

		smpscfg_12.DrumLib.DrumData[0].Type = DRMTYPE_NONE;
		for (i = 1; i < smpscfg_12.DrumLib.DrumCount; i++)
		{
			if (!S2DrumList[i][0])
			{
				smpscfg_12.DrumLib.DrumData[i].Type = DRMTYPE_NONE;
			}
			else
			{
				smpscfg_12.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
				smpscfg_12.DrumLib.DrumData[i].DrumID = S2DrumList[i][0] - 0x81;	// set DAC sample
				smpscfg_12.DrumLib.DrumData[i].PitchOvr = S2DrumList[i][1];
			}
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_MODENV_S3K), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadEnvelopeData_Mem(dataSize, dataPtr, &ModEnvs_S3K);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_MODENV_S3PROTO), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadEnvelopeData_Mem(dataSize, dataPtr, &ModEnvs_S3Proto);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_VOLENV_S3), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadEnvelopeData_Mem(dataSize, dataPtr, &VolEnvs_S3);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_VOLENV_SK), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadEnvelopeData_Mem(dataSize, dataPtr, &VolEnvs_SK);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_VOLENV_S2), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadEnvelopeData_Mem(dataSize, dataPtr, &smpscfg_12.VolEnvs);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_FM_DRUMS), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadDrumTracks_Mem(dataSize, dataPtr, &smpscfg_3K.FMDrums, 0x01);	// 0x01 - FM drums

		smpscfg_3K.VolEnvs = VolEnvs_SK;

		GenerateDACDrv(&smpscfg_3K.DACDrv, (IDR_DAC_A0_S3D - IDR_DAC_81) + 1, IDR_DAC_81, 297,
			(unsigned int)LengthOfArray(DACFiles), DACFiles);

		GenerateDACDrv(&smpscfg_12.DACDrv, (IDR_DAC_87_S2 - IDR_DAC_81_S2) + 1, IDR_DAC_81_S2, 288,
			(unsigned int)LengthOfArray(DACFiles_S2), DACFiles_S2);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_INSSET), _T("MISC"));
		LoadGlobalInstrumentLib_Mem((UINT16)SizeofResource(moduleHandle, hres),
			(UINT8 *)LockResource(LoadResource(moduleHandle, hres)), &smpscfg_3K);
		/*smpscfg_3K.GblIns.alloc = 0x00;
		smpscfg_3K.GblIns.Len = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg_3K.GblIns.Data = (UINT8 *)LockResource(LoadResource(moduleHandle, hres));

		UINT16 InsCount = smpscfg_3K.GblIns.Len / smpscfg_3K.InsRegCnt;
		smpscfg_3K.GblInsLib.InsCount = InsCount;
		smpscfg_3K.GblInsLib.InsPtrs = NULL;
		if (InsCount)
		{
			smpscfg_3K.GblInsLib.InsPtrs = new UINT8*[InsCount];
			UINT16 CurPos = 0x0000;
			for (UINT16 CurIns = 0x00; CurIns < InsCount; CurIns ++, CurPos += smpscfg_3K.InsRegCnt)
				smpscfg_3K.GblInsLib.InsPtrs[CurIns] = &smpscfg_3K.GblIns.Data[CurPos];
		}*/
		smpscfg_3K.GblInsBase = 0x17D8;

		smpscfg_12.GblIns.alloc = 0x00;
		smpscfg_12.GblIns.Len = 0x0000;
		smpscfg_12.GblIns.Data = NULL;
		smpscfg_12.GblInsLib.InsCount = 0;
		smpscfg_12.GblInsLib.InsPtrs = NULL;

		DAC_Reset();
		InitAudioOutput();
		{
			const char* const AudAPIList[] = { "WASAPI", "XAudio2", "DirectSound", "WinMM" };
			unsigned int curADrv;
			UINT8 retVal;

			for (curADrv = 0; curADrv < 4; curADrv++)
			{
				retVal = QueryDeviceParams(AudAPIList[curADrv], &AudioCfg);
				if (retVal)
					continue;
				AudioCfg.AudAPIName = (char*)AudAPIList[curADrv];
				AudioCfg.AudioBufs = 8;
				AudioCfg.AudioBufSize = (curADrv < 3) ? 4 : 8;	// WinMM: use 8+ buffers
				AudioCfg.Volume = 1.0f;

				retVal = StartAudioOutput();
				if (!retVal)
					break;
			}
		}
		InitDriver();
		SMPSExtra_SetCallbacks(SMPSCB_STOP, &NotifySongStopped);
		SmpsReloadState = SmpsGetVariable(SMPSVAR_RESTORE_REQ);
		SmpsMusicSaveState = SmpsGetVariable(SMPSVAR_MUSSTATE_USE);


		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_LoadSong(short id)
	{
		int newid = id;
		if ((size_t)newid >= songs.size())
			return FALSE;
		const SongInfo *song = &songs[newid];
		switch (song->mode)
		{
		case TrackMode_S1:
		case TrackMode_S2B:
		case TrackMode_S2:
			cursmpscfg = &smpscfg_12;
			switch (song->mode)
			{
			case TrackMode_S1:
				cursmpscfg->PtrFmt = PTRFMT_68K;
				cursmpscfg->InsMode = INSMODE_DEF;
				LoadRegisterList(cursmpscfg, (UINT8)LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
				cursmpscfg->TempoMode = TEMPO_TIMEOUT;
				break;
			case TrackMode_S2B:
				cursmpscfg->PtrFmt = PTRFMT_Z80;
				cursmpscfg->InsMode = INSMODE_HW;
				LoadRegisterList(cursmpscfg, (UINT8)LengthOfArray(INSOPS_HARDWARE), INSOPS_HARDWARE);
				cursmpscfg->TempoMode = TEMPO_TIMEOUT;
				break;
			case TrackMode_S2:
				cursmpscfg->PtrFmt = PTRFMT_Z80;
				cursmpscfg->InsMode = INSMODE_HW;
				LoadRegisterList(cursmpscfg, (UINT8)LengthOfArray(INSOPS_HARDWARE), INSOPS_HARDWARE);
				cursmpscfg->TempoMode = TEMPO_OVERFLOW2;
				break;
			}
			break;
			//default:
		case TrackMode_SK:
		case TrackMode_S3:
		case TrackMode_S3P:
		case TrackMode_S3D:
			DRUM_LIB * drumLib;

			cursmpscfg = &smpscfg_3K;
			drumLib = &cursmpscfg->DrumLib;
			if (song->mode == TrackMode_S3P)
			{
				cursmpscfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S3PROTO;
				cursmpscfg->ModEnvs = ModEnvs_S3Proto;
				cursmpscfg->VolEnvs = VolEnvs_S3;
			}
			else if (song->mode == TrackMode_S3)
			{
				cursmpscfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S3K;
				cursmpscfg->ModEnvs = ModEnvs_S3K;
				cursmpscfg->VolEnvs = VolEnvs_S3;
			}
			else
			{
				cursmpscfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S3K;
				cursmpscfg->ModEnvs = ModEnvs_S3K;
				cursmpscfg->VolEnvs = VolEnvs_SK;
			}

			UINT8 i;
			if (fmdrum_on)
			{
				// Sonic 3/K/3D - FM drums
				for (i = 1; i <= LengthOfArray(FMDrumList); i++)
				{
					if (FMDrumList[i - 1])
					{
						drumLib->DrumData[i].Type = DRMTYPE_FM;
						drumLib->DrumData[i].DrumID = FMDrumList[i - 1] - 0x81;
					}
				}
				if (song->mode == TrackMode_S3D)
				{
					drumLib->DrumData[0x9F - 0x80].Type = DRMTYPE_FM;
					drumLib->DrumData[0x9F - 0x80].DrumID = 0x86 - 0x81;
					drumLib->DrumData[0xA0 - 0x80].Type = DRMTYPE_DAC;
					drumLib->DrumData[0xA0 - 0x80].DrumID = S3D_DAC9F_ID_BASE + (0xA0 - 0x9F);
				}
			}
			else if (song->mode == TrackMode_S3D)
			{
				// Sonic 3D DAC drums
				for (i = 0x01; i < 0x1F; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = i - 1;	// set DAC sample
				}
				for (; i <= 0x20; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = S3D_DAC9F_ID_BASE + (i - 0x1F);	// S3D sounds 9F/A0 are C5/C6 here.
				}
				for (; i < drumLib->DrumCount; i++)
					drumLib->DrumData[i].Type = DRMTYPE_NONE;
			}
			else
			{
				// Sonic 3/K DAC drums
				for (i = 0x01; i < drumLib->DrumCount; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = i - 1;	// set DAC sample
				}
				if (song->mode == TrackMode_S3P)
				{
					drumLib->DrumData[0x9B - 0x80].DrumID = S3P_DAC9B_ID_BASE;
					drumLib->DrumData[0xB2 - 0x80].DrumID = S3_DACB2_ID_BASE + 0;
					drumLib->DrumData[0xB3 - 0x80].DrumID = S3_DACB2_ID_BASE + 1;
				}
				else if (song->mode == TrackMode_S3)
				{
					drumLib->DrumData[0xB2 - 0x80].DrumID = S3_DACB2_ID_BASE + 0;
					drumLib->DrumData[0xB3 - 0x80].DrumID = S3_DACB2_ID_BASE + 1;
				}
			}
			break;
		}	// end switch (song->mode)

		memset(&cursmps, 0x00, sizeof(SMPS_SET));
		cursmps.Cfg = cursmpscfg;
		cursmps.SeqBase = song->base;
		cursmps.Seq.alloc = 0x00;
		cursmps.Seq.Len = song->length;
		cursmps.Seq.Data = song->data;
		PreparseSMPSFile(&cursmps);
		if (!default1UpHandling)
		{
			cursmps.SeqFlags &= ~SEQFLG_NEED_SAVE;
			ClearSavedStates();
		}
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_PlaySong()
	{
		if (cursmpscfg == NULL)
			return TRUE;

		ThreadSync(1);

		FrameDivider = 60;
		PlayMusic(&cursmps);

		ThreadSync(0);
		PauseStream(0);
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_LoadAndPlaySong(short id)
	{
		return SMPS_LoadSong(id) && SMPS_PlaySong();
	}

	SMPSPlay_API BOOL SMPS_StopSong()
	{
		ThreadSync(1);
		StopAllSound();
		ThreadSync(0);
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_PauseSong()
	{
		PauseResumeMusic(1);
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_ResumeSong()
	{
		PauseResumeMusic(0);
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_SetSongTempo(double multiplier)
	{
		FrameDivider = (UINT16)(60 * multiplier + 0.5);
		return TRUE;
	}

	SMPSPlay_API BOOL SMPS_DeInitializeDriver()
	{
		SMPSExtra_SetCallbacks(SMPSCB_OFF, NULL);	// doing callbacks now can cause crashes
		ThreadSync(1);
		StopAllSound();

		DeinitDriver();
		//StopAudioOutput();	// It's already too late to do this.
		//DeinitAudioOutput();
		{
			delete[] smpscfg_3K.DrumLib.DrumData;
			delete[] smpscfg_12.DrumLib.DrumData;
			FreeEnvelopeData(&ModEnvs_S3K);
			FreeEnvelopeData(&ModEnvs_S3Proto);
			FreeEnvelopeData(&smpscfg_12.ModEnvs);
			FreeEnvelopeData(&VolEnvs_S3);
			FreeEnvelopeData(&VolEnvs_SK);
			FreeEnvelopeData(&smpscfg_12.VolEnvs);
			//FreeDACData(&smpscfg_3K.DACDrv);
			{
				delete[] smpscfg_3K.DACDrv.Smpls;
				delete[] smpscfg_3K.DACDrv.Cfg.Algos;
				delete[] smpscfg_3K.DACDrv.SmplTbl;
			}
			//FreeDACData(&smpscfg_12.DACDrv);
			{
				delete[] smpscfg_12.DACDrv.Smpls;
				delete[] smpscfg_12.DACDrv.Cfg.Algos;
				delete[] smpscfg_12.DACDrv.SmplTbl;
			}
			FreeDrumTracks(&smpscfg_3K.FMDrums);
			FreeDrumTracks(&smpscfg_3K.PSGDrums);
			FreeGlobalInstrumentLib(&smpscfg_3K);
			FreeGlobalInstrumentLib(&smpscfg_12);
		}
		return TRUE;
	}

	static void(*SongStoppedCallback)() = NULL;

	SMPSPlay_API void SMPS_RegisterSongStoppedCallback(void(*callback)())
	{
		SongStoppedCallback = callback;
	}

	SMPSPlay_API void SMPS_FadeOutSong()
	{
		FadeOutMusic();
	}

	SMPSPlay_API void SMPS_FadeInSong()
	{
		FadeInMusic();
	}

	SMPSPlay_API void SMPS_SetDefault1UpHandling(bool enable)
	{
		default1UpHandling = enable;
	}

	SMPSPlay_API const char **SMPS_GetSongNames(unsigned int &count)
	{
		count = (unsigned int)songnames.size();
		return songnames.data();
	}

	SMPSPlay_API void SMPS_SetVolume(double volume)
	{
		OutputVolume = (INT32)(volume * 0x100 + 0.5);
	}

	SMPSPlay_API void SMPS_SetWaveLogPath(const char *logfile)
	{
		if (AudioCfg.WaveLogPath)
		{
			free(AudioCfg.WaveLogPath);
			AudioCfg.WaveLogPath = NULL;
		}
		if (logfile)
		{
			AudioCfg.WaveLogPath = _strdup(logfile);
			AudioCfg.LogWave = 1;
		}
		else
		{
			AudioCfg.LogWave = 0;
		}
	}

	SMPSPlay_API void SMPS_RegisterSongLoopCallback(SMPS_CB_SIGNAL func)
	{
		SMPSExtra_SetCallbacks(SMPSCB_LOOP, func);
	}

	static void NotifySongStopped(void)
	{
		if (SongStoppedCallback != NULL)
			SongStoppedCallback();
	}

	SMPSPlay_API void SMPS_EnableFMDrums(bool enable)
	{
		fmdrum_on = enable;
	}
}