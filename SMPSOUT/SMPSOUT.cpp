// SMPSOUT.cpp : Defines the exported functions for the DLL application.
//

#ifdef _MSC_VER
#include <tchar.h>
#else
#define _T(x) L ## x
#endif
#include "Engine\smps.h"
#include "Engine\smps_commands.h"
#include "Sound.h"
#include "Stream.h"
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

HMODULE moduleHandle;
HWND gameWindow;
extern "C"
{
	extern volatile bool PauseThread;	// from Stream.c
	extern volatile bool ThreadPauseConfrm;
	extern volatile bool CloseThread;
	extern UINT32 SampleRate;	// from Sound.c
	extern UINT16 FrameDivider;
	extern UINT32 SmplsPerFrame;
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

dacentry DACFiles[] = {
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
	{ IDR_DAC_B4, 0x0B },
	{ IDR_DAC_9F_S3D, 0x01 },
	{ IDR_DAC_A0_S3D, 0x12 }
};
#define S3D_ID_BASE	(0xC5 - 0x81)

UINT8 FMDrumList[] = {
	// 0    1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x86, 0x87, 0x82, 0x83, 0x82, 0x84, 0x82, 0x85,	// 81..8F
	0x82, 0x83, 0x84, 0x85, 0x82, 0x83, 0x84, 0x85, 0x82, 0x83, 0x84, 0x81, 0x88, 0x86, 0x81, 0x81,	// 90..9F
	0x87, 0x87, 0x87, 0x81, 0x86, 0x00, 0x00, 0x86, 0x86, 0x87, 0x87, 0x81, 0x85, 0x82, 0x83, 0x82,	// A0..AF
	0x84, 0x87, 0x82, 0x84, 0x00, 0x00, 0x87, 0x86, 0x86, 0x86, 0x87, 0x87, 0x81, 0x86, 0x00, 0x81,	// B0..BF
	0x86, 0x00, 0x00, 0x00, 0x00                                                                   	// C0..C4
};

dacentry DACFiles_S2[] = {
	{ IDR_DAC_81_S2, 0x17 },
	{ IDR_DAC_82_S2, 0x01 },
	{ IDR_DAC_83_S2, 0x06 },
	{ IDR_DAC_84_S2, 0x08 },
	{ IDR_DAC_85_S2, 0x1B },
	{ IDR_DAC_86_S2, 0x0A },
	{ IDR_DAC_87_S2, 0x1B }
};

UINT8 S2DrumList[][2] =
{	{0x00, 0x00},
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

static const UINT8 DefDPCMData[] =
{	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
	0x80, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0};

static const UINT8 FMCHN_ORDER[] = {0x16, 0, 1, 2, 4, 5, 6};

static const UINT16 DEF_FMFREQ_VAL[13] =
{0x25E, 0x284, 0x2AB, 0x2D3, 0x2FE, 0x32D, 0x35C, 0x38F, 0x3C5, 0x3FF, 0x43C, 0x47C, 0x4C0};

static const UINT16 DEF_PSGFREQ_68K_VAL[] =
{	0x356, 0x326, 0x2F9, 0x2CE, 0x2A5, 0x280, 0x25C, 0x23A, 0x21A, 0x1FB, 0x1DF, 0x1C4,
	0x1AB, 0x193, 0x17D, 0x167, 0x153, 0x140, 0x12E, 0x11D, 0x10D, 0x0FE, 0x0EF, 0x0E2,
	0x0D6, 0x0C9, 0x0BE, 0x0B4, 0x0A9, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071,
	0x06B, 0x065, 0x05F, 0x05A, 0x055, 0x050, 0x04B, 0x047, 0x043, 0x040, 0x03C, 0x039,
	0x036, 0x033, 0x030, 0x02D, 0x02B, 0x028, 0x026, 0x024, 0x022, 0x020, 0x01F, 0x01D,
	0x01B, 0x01A, 0x018, 0x017, 0x016, 0x015, 0x013, 0x012, 0x011, 0x000};

static const UINT16 DEF_PSGFREQ_Z80_T2_VAL[] =
{	0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3F7, 0x3BE, 0x388,
	0x356, 0x326, 0x2F9, 0x2CE, 0x2A5, 0x280, 0x25C, 0x23A, 0x21A, 0x1FB, 0x1DF, 0x1C4,
	0x1AB, 0x193, 0x17D, 0x167, 0x153, 0x140, 0x12E, 0x11D, 0x10D, 0x0FE, 0x0EF, 0x0E2,
	0x0D6, 0x0C9, 0x0BE, 0x0B4, 0x0A9, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071,
	0x06B, 0x065, 0x05F, 0x05A, 0x055, 0x050, 0x04B, 0x047, 0x043, 0x040, 0x03C, 0x039,
	0x036, 0x033, 0x030, 0x02D, 0x02B, 0x028, 0x026, 0x024, 0x022, 0x020, 0x01F, 0x01D,
	0x01B, 0x01A, 0x018, 0x017, 0x016, 0x015, 0x013, 0x012, 0x011, 0x010, 0x000, 0x000};

static const UINT16 FM3FREQS[] = { 0, 0x132, 0x18E, 0x1E4, 0x234, 0x27E, 0x2C2, 0x2F0 };

static const CMD_FLAGS CMDFLAGS_S12[] = {
	{ CF_PANAFMS, CFS_PAFMS_PAN, 0x02 },
	{ CF_DETUNE, 0x00, 0x02 },
	{ CF_SET_COMM, 0x00, 0x02 },
	{ CF_RETURN, 0x00, 0x01 },
	{ CF_TRK_END, 0x00, 0x01 },	// actually CF_FADE_IN_SONG, but that's not supported yet
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
	{ CF_IGNORE, 0x00, 0x03, 0x01 },
	{ CF_RAW_FREQ, 0x00, 0x02 },
	{ CF_SPC_FM3, 0x00, 0x05 },
	{ CF_META_CF, 0x00, 0x01 }
};

static const CMD_FLAGS CMDMETAFLAGS[] = {
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
0x40, 0x48, 0x44, 0x4C,
0x00};	// The 0 is required by the SMPS routines to terminate the array.

static const UINT8 INSOPS_HARDWARE[] =
{	0xB0,
0x30, 0x34, 0x38, 0x3C,
0x50, 0x54, 0x58, 0x5C,
0x60, 0x64, 0x68, 0x6C,
0x70, 0x74, 0x78, 0x7C,
0x80, 0x84, 0x88, 0x8C,
0x40, 0x44, 0x48, 0x4C,
0x00};	// The 0 is required by the SMPS routines to terminate the array.

enum DriverMode {
	SmpsDrv_S3K,
	SmpsDrv_S12
};

struct SongInfo { UINT8 *data; UINT16 length; UINT16 base; unsigned char mode; };

vector<SongInfo> songs(SongCount);

vector<const char *> customsongs;

#define DataRef(type,name,addr) type &name = *(type *)addr

DataRef(unsigned int, GameSelection, 0x831188);
DataRef(unsigned char, reg_d0, 0x8549A4);
DataRef(unsigned short, Ending_running_flag, 0x8FFEF72);
DataRef(unsigned char, Game_mode, 0x8FFF600);
DataRef(unsigned char, Super_Tails_flag, 0x8FFF667);
DataRef(unsigned short, Current_zone_and_act, 0x8FFFE10);
DataRef(unsigned char, Super_Sonic_Knux_flag, 0x8FFFE19);
DataRef(unsigned short, Saved_zone_and_act, 0x8FFFE2C);
DataRef(unsigned short, Player_mode, 0x8FFFF08);

#define GameModeID_SegaScreen 0
#define GameModeID_TitleScreen 4
#define GameModeID_Demo 8
#define GameModeID_Level 0xC
#define GameModeID_SegaScreen2 0x10
#define GameModeID_ContinueScreen 0x14
#define GameModeID_SegaScreen3 0x18
#define GameModeID_LevelSelect 0x1C
#define GameModeID_S3Credits 0x20
#define GameModeID_LevelSelect2 0x24
#define GameModeID_LevelSelect3 0x28
#define GameModeID_BlueSpheresTitle 0x2C
#define GameModeID_BlueSpheresDifficulty 0x2C
#define GameModeID_BlueSpheresResults 0x30
#define GameModeID_SpecialStage 0x34
#define GameModeID_CompetitionMenu 0x38
#define GameModeID_CompetitionPlayerSelect 0x3C
#define GameModeID_CompetitionLevelSelect 0x40
#define GameModeID_CompetitionResults 0x44
#define GameModeID_SpecialStageResults 0x48
#define GameModeID_SaveScreen 0x4C
#define GameModeID_TimeAttackRecords 0x50

#define flying_battery_zone 4
#define mushroom_hill_zone 7
#define gumball_bonus 0x13
#define glowing_spheres_bonus 0x14
#define slot_bonus 0x15
#define ending 0x0D01
#define hidden_palace_zone 0x1601
#define hidden_palace_shrine 0x1701

class MidiInterfaceClass
{
public:
	virtual BOOL initialize(HWND hwnd) = 0; // hwnd = game window
	virtual BOOL load_song(short id, unsigned int bgmmode) = 0; // id = song to be played + 1 (well, +1 compared to the sound test id, it's the ID of the song in the MIDIOUT.DLL's resources); bgmmode = 0 for FM synth, 1 for General MIDI
	virtual BOOL play_song() = 0;
	virtual BOOL stop_song() = 0;
	virtual BOOL pause_song() = 0;
	virtual BOOL unpause_song() = 0;
	virtual BOOL set_song_tempo(unsigned int pct) = 0; // pct = percentage of delay between beats the song should be set to. lower = faster tempo
};

bool EnableSKCHacks = true;
const char *const INISections[] = { "S3K", "S&K", "S3" };

class SMPSInterfaceClass : MidiInterfaceClass
{
	SMPS_CFG smpscfg_3K;
	SMPS_CFG smpscfg_12;
	SMPS_CFG* cursmpscfg;
	ENV_LIB VolEnvs_S3;
	ENV_LIB VolEnvs_SK;
	bool fmdrum_on;
	short trackSettings[TrackCount], s3TrackSettings[TrackCount], skTrackSettings[TrackCount];
	bool trackMIDI;
	MidiInterfaceClass *MIDIFallbackClass;

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

	static void LoadRegisterList(SMPS_CFG* SmpsCfg, UINT8 RegCnt, const UINT8* RegPtr)
	{
		UINT8 CurReg;
		UINT8 RegTL_Idx;

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

		SmpsCfg->InsRegCnt = RegCnt;
		SmpsCfg->InsRegs = RegPtr;
		if (RegTL_Idx == 0xFF)
			SmpsCfg->InsReg_TL = NULL;
		else
			SmpsCfg->InsReg_TL = &RegPtr[RegTL_Idx];

		return;
	}

	static void LoadSettings(UINT8 Mode, SMPS_CFG* SmpsCfg)
	{
		switch(Mode)
		{
		case SmpsDrv_S12:
			SmpsCfg->PtrFmt = PTRFMT_Z80;

			SmpsCfg->InsMode = INSMODE_DEF;
			LoadRegisterList(SmpsCfg, LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
			SmpsCfg->FMChnCnt = LengthOfArray(FMCHN_ORDER);
			memcpy(SmpsCfg->FMChnList, FMCHN_ORDER, 7);

			SmpsCfg->TempoMode = TEMPO_TIMEOUT;
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
			SmpsCfg->PSGFreqCnt = LengthOfArray(DEF_PSGFREQ_68K_VAL);
			SmpsCfg->FM3Freqs = (UINT16*)FM3FREQS;
			SmpsCfg->FM3FreqCnt = LengthOfArray(FM3FREQS);

			SmpsCfg->EnvCmds[0] = ENVCMD_HOLD;

			SmpsCfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS_S12;
			SmpsCfg->CmdList.FlagBase = 0xE0;
			SmpsCfg->CmdList.FlagCount = LengthOfArray(CMDFLAGS);

			SmpsCfg->CmdMetaList.CmdData = NULL;
			SmpsCfg->CmdMetaList.FlagBase = 0x00;
			SmpsCfg->CmdMetaList.FlagCount = 0;
			break;
		case SmpsDrv_S3K:
			SmpsCfg->PtrFmt = PTRFMT_Z80;

			SmpsCfg->InsMode = INSMODE_DEF;
			LoadRegisterList(SmpsCfg, LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
			SmpsCfg->FMChnCnt = LengthOfArray(FMCHN_ORDER);
			memcpy(SmpsCfg->FMChnList, FMCHN_ORDER, 7);

			SmpsCfg->TempoMode = TEMPO_OVERFLOW;
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
			SmpsCfg->PSGFreqCnt = LengthOfArray(DEF_PSGFREQ_Z80_T2_VAL);
			SmpsCfg->FM3Freqs = (UINT16*)FM3FREQS;
			SmpsCfg->FM3FreqCnt = LengthOfArray(FM3FREQS);

			SmpsCfg->EnvCmds[0] = ENVCMD_RESET;
			SmpsCfg->EnvCmds[1] = ENVCMD_HOLD;
			SmpsCfg->EnvCmds[2] = ENVCMD_LOOP;
			SmpsCfg->EnvCmds[3] = ENVCMD_STOP;
			SmpsCfg->EnvCmds[4] = ENVCMD_CHGMULT;

			SmpsCfg->CmdList.CmdData = (CMD_FLAGS*)CMDFLAGS;
			SmpsCfg->CmdList.FlagBase = 0xE0;
			SmpsCfg->CmdList.FlagCount = LengthOfArray(CMDFLAGS);

			SmpsCfg->CmdMetaList.CmdData = (CMD_FLAGS*)CMDMETAFLAGS;
			SmpsCfg->CmdMetaList.FlagBase = 0x00;
			SmpsCfg->CmdMetaList.FlagCount = LengthOfArray(CMDMETAFLAGS);
			break;
		}
	}

	static void GetEnvelopeData(UINT32 DataLen, const UINT8* Data, ENV_LIB *EnvLib)
	{
		UINT32 CurPos;
		UINT8 CurEnv;
		ENV_DATA* TempEnv;

		CurPos = 0x07;
		EnvLib->EnvCount = Data[CurPos];	CurPos ++;
		EnvLib->EnvData = new ENV_DATA[EnvLib->EnvCount];
		for (CurEnv = 0x00; CurEnv < EnvLib->EnvCount; CurEnv ++)
		{
			TempEnv = &EnvLib->EnvData[CurEnv];
			CurPos += 0x01 + Data[CurPos];
			if (CurPos >= DataLen)
			{
				EnvLib->EnvCount = CurEnv;
				break;
			}

			TempEnv->Len = Data[CurPos];	CurPos ++;
			TempEnv->Data = new UINT8[TempEnv->Len];
			memcpy(TempEnv->Data, &Data[CurPos], TempEnv->Len);
			CurPos += TempEnv->Len;
		}
	}

	static UINT8 LoadDrumTracks(UINT32 DataLen, const UINT8* Data, DRUM_TRK_LIB* DrumLib, UINT8 DrumMode)
	{
		UINT8 InsCount;
		UINT16 DrumOfs;
		UINT16 InsOfs;
		UINT16 InsBaseOfs;
		UINT16 BaseOfs;
		UINT8 CurItm;
		UINT16 CurPos;
		UINT16 TempOfs;

		if (Data[0x04] != DrumMode)
			return 0xC0;	// wrong drum mode
		switch(DrumMode)
		{
		case 0x00:	// PSG drums (without Instrument lib.)
			if (DataLen < 0x0A)
				return 0x80;	// invalid file
			DrumLib->DrumCount = Data[0x05];
			DrumOfs = ReadLE16(&Data[0x06]);
			DrumLib->DrumBase = ReadLE16(&Data[0x08]);
			InsCount = 0x00;
			InsOfs = 0x0000;
			break;
		case 0x01:	// FM drums (with Instrument lib.)
			if (DataLen < 0x10)
				return 0x80;	// invalid file
			DrumLib->DrumCount = Data[0x05];
			DrumOfs = ReadLE16(&Data[0x06]);
			DrumLib->DrumBase = ReadLE16(&Data[0x08]);
			InsCount = Data[0x0B];
			InsOfs = ReadLE16(&Data[0x0C]);
			InsBaseOfs = ReadLE16(&Data[0x0E]);
			break;
		}
		if (! DrumOfs)
			return 0xC1;	// invalid drum offset

		BaseOfs = DrumOfs;
		if (InsOfs && InsOfs < BaseOfs)
			BaseOfs = InsOfs;

		DrumLib->DataLen = DataLen - BaseOfs;
		DrumLib->Data = (UINT8*)malloc(DrumLib->DataLen);
		memcpy(DrumLib->Data, &Data[BaseOfs], DrumLib->DataLen);

		DrumLib->DrumList = (UINT16*)malloc(DrumLib->DrumCount * sizeof(UINT16));
		CurPos = DrumOfs - BaseOfs;
		DrumLib->DrumBase -= CurPos;
		for (CurItm = 0x00; CurItm < DrumLib->DrumCount; CurItm ++, CurPos += 0x02)
			DrumLib->DrumList[CurItm] = ReadLE16(&DrumLib->Data[CurPos]);

		DrumLib->InsLib.InsCount = InsCount;
		DrumLib->InsLib.InsPtrs = NULL;
		if (InsCount)
		{
			DrumLib->InsLib.InsPtrs = (UINT8**)malloc(InsCount * sizeof(UINT8*));
			CurPos = InsOfs - BaseOfs;
			InsBaseOfs -= CurPos;
			for (CurItm = 0x00; CurItm < InsCount; CurItm ++, CurPos += 0x02)
			{
				TempOfs = ReadLE16(&DrumLib->Data[CurPos]) - InsBaseOfs;
				if (TempOfs >= BaseOfs && TempOfs < DrumLib->DataLen - 0x10)
					DrumLib->InsLib.InsPtrs[CurItm] = &DrumLib->Data[TempOfs];
				else
					DrumLib->InsLib.InsPtrs[CurItm] = NULL;
			}
		}

		return 0x00;
	}

	static void FreeDrumTracks(DRUM_TRK_LIB* DrumLib)
	{
		if (DrumLib->DrumList == NULL)
			return;

		DrumLib->DrumCount = 0x00;
		free(DrumLib->DrumList);
		DrumLib->DrumList = NULL;

		if (DrumLib->InsLib.InsPtrs != NULL)
		{
			DrumLib->InsLib.InsCount = 0x00;
			free(DrumLib->InsLib.InsPtrs);
			DrumLib->InsLib.InsPtrs = NULL;
		}

		DrumLib->DataLen = 0x00;
		free(DrumLib->Data);
		DrumLib->DrumList = NULL;

		return;
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

#ifdef INISUPPORT
	void ReadSettings(const IniGroup *settings, short *trackSettings)
	{
		for (int i = 0; i < TrackCount; i++)
		{
			string value = settings->getString(TrackOptions[i].name);
			if (value == "ByCharacter")
			{
				trackSettings[i] = MusicID_ByCharacter;
				continue;
			}
			else if (value == "ByZone")
			{
				trackSettings[i] = MusicID_ByZone;
				continue;
			}
			else if (value == "MIDI")
			{
				trackSettings[i] = MusicID_MIDI;
				continue;
			}
			else if (value == "Default")
				continue;
			else if (TrackOptions[i].optioncount >= 2)
			{
				if (value == "Random")
				{
					trackSettings[i] = MusicID_Random;
					continue;
				}
				else
				{
					bool found = false;
					for (int j = 0; j < TrackOptions[i].optioncount; j++)
						if (value == TrackOptions[i].options[j].text)
						{
							trackSettings[i] = TrackOptions[i].options[j].id;
							found = true;
							break;
						}
					if (found) continue;
				}
			}
			for (unsigned int j = 0; j < customsongs.size(); j++)
				if (value == customsongs[j])
				{
					trackSettings[i] = SongCount + j;
					break;
				}
		}
	}
#endif

public:
	SMPSInterfaceClass() { }

	BOOL initialize(HWND hwnd)
	{
		HRSRC hres;
		UINT8* dataPtr;
		UINT32 dataSize;
		unsigned int i;

		ZeroMemory(&smpscfg_3K, sizeof(smpscfg_3K));
		ZeroMemory(&smpscfg_12, sizeof(smpscfg_12));
		cursmpscfg = NULL;
		fmdrum_on = false;

		for (int i = 0; i < SongCount; i++)
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MUSIC_1 + i), _T("MUSIC"));
			songs[i].mode = MusicFiles[i].mode;
			songs[i].base = MusicFiles[i].base;
			songs[i].length = (UINT16)SizeofResource(moduleHandle, hres);
			songs[i].data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		}

#ifdef INISUPPORT
		IniFile custsongs(_T("songs_cust.ini"));
			
		for (auto iter = custsongs.begin(); iter != custsongs.end(); iter++)
		{
			if (iter->first.empty()) continue;
			SongInfo si = { };
			string str = iter->second->getString("Type");
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
			si.base = iter->second->getHexInt("Offset");
			FILE *fi;
			fopen_s(&fi, iter->second->getString("File").c_str(), "rb");
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
				customsongs.push_back(buf);
			}
		}
#endif

		if (EnableSKCHacks)
		{
			HMODULE midimodule = LoadLibrary(_T("MIDIOUTY.DLL"));
			if (midimodule)
			{
				MIDIFallbackClass = ((MidiInterfaceClass *(*)())GetProcAddress(midimodule, "GetMidiInterface"))();
				MIDIFallbackClass->initialize(hwnd);
			}

			gameWindow = hwnd;

			memset(&trackSettings, MusicID_Default, sizeof(short) * TrackCount);

#ifdef INISUPPORT
			IniFile settings(_T("SMPSOUT.ini"));
			fmdrum_on = settings.getBool("", "FMDrums");

			const IniGroup *group = settings.getGroup("");
			if (group != nullptr)
				ReadSettings(group, trackSettings);

			memcpy(s3TrackSettings, trackSettings, sizeof(short) * TrackCount);
			memcpy(skTrackSettings, trackSettings, sizeof(short) * TrackCount);

			group = settings.getGroup(INISections[GameSelection]);
			if (group != nullptr)
				ReadSettings(group, trackSettings);

			group = settings.getGroup("S3");
			if (group != nullptr)
				ReadSettings(group, s3TrackSettings);
			if (s3TrackSettings[MusicID_Midboss] == MusicID_Default)
				s3TrackSettings[MusicID_Midboss] = MusicID_S3Midboss;
			if (s3TrackSettings[MusicID_Continue] == MusicID_Default)
				s3TrackSettings[MusicID_Continue] = MusicID_S3Continue;

			group = settings.getGroup("S&K");
			if (group != nullptr)
				ReadSettings(group, skTrackSettings);

#else
			fmdrum_on = true;
#endif
			if (trackSettings[MusicID_HiddenPalace] == MusicID_Default)
				trackSettings[MusicID_HiddenPalace] = MusicID_LavaReef2;
			if (trackSettings[MusicID_Ending] == MusicID_Default)
				trackSettings[MusicID_Ending] = MusicID_SkySanctuary;
			if (trackSettings[MusicID_BlueSphereTitle] == MusicID_Default)
				trackSettings[MusicID_BlueSphereTitle] = GameSelection == 2 ? MusicID_S3Continue : MusicID_Continue;
			if (trackSettings[MusicID_BlueSphereDifficulty] == MusicID_Default)
				trackSettings[MusicID_BlueSphereDifficulty] = MusicID_SKInvincibility;
			if (trackSettings[MusicID_TimeAttackRecords] == MusicID_Default)
				trackSettings[MusicID_TimeAttackRecords] = GameSelection == 2 ? MusicID_S3Continue : MusicID_Continue;
			if (GameSelection == 2)
			{
				if (trackSettings[MusicID_Midboss] == MusicID_Default)
					trackSettings[MusicID_Midboss] = MusicID_S3Midboss;
				if (trackSettings[MusicID_Continue] == MusicID_Default)
					trackSettings[MusicID_Continue] = MusicID_S3Continue;
			}
		}
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
		smpscfg_12.DrumLib.DrumCount = LengthOfArray(S2DrumList);
		smpscfg_12.DrumLib.DrumData = new DRUM_DATA[smpscfg_12.DrumLib.DrumCount];
		ZeroMemory(smpscfg_12.DrumLib.DrumData, sizeof(DRUM_DATA) * smpscfg_12.DrumLib.DrumCount);

		smpscfg_12.DrumLib.DrumData[0].Type = DRMTYPE_NONE;
		for (i = 1; i < smpscfg_12.DrumLib.DrumCount; i ++)
		{
			if (! S2DrumList[i][0])
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

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_MODULAT), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &smpscfg_3K.ModEnvs);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG_S3), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &VolEnvs_S3);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG_SK), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &VolEnvs_SK);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG_S2), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &smpscfg_12.VolEnvs);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_FM_DRUMS), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadDrumTracks(dataSize, dataPtr, &smpscfg_3K.FMDrums, 0x01);	// 0x01 - FM drums

		smpscfg_3K.VolEnvs = VolEnvs_SK;

		ZeroMemory(&smpscfg_3K.DACDrv, sizeof(smpscfg_3K.DACDrv));
		smpscfg_3K.DACDrv.SmplCount = (IDR_DAC_A0_S3D - IDR_DAC_81) + 1;	// all DAC sounds
		smpscfg_3K.DACDrv.SmplAlloc = smpscfg_3K.DACDrv.SmplCount;
		smpscfg_3K.DACDrv.Smpls = new DAC_SAMPLE[smpscfg_3K.DACDrv.SmplAlloc];
		ZeroMemory(smpscfg_3K.DACDrv.Smpls, sizeof(DAC_SAMPLE) * smpscfg_3K.DACDrv.SmplAlloc);

		for (i = 0; i < smpscfg_3K.DACDrv.SmplCount; i++)
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_81 + i), _T("DAC"));
			smpscfg_3K.DACDrv.Smpls[i].Compr = COMPR_DPCM;
			smpscfg_3K.DACDrv.Smpls[i].DPCMArr = DefDPCMData;
			smpscfg_3K.DACDrv.Smpls[i].Size = SizeofResource(moduleHandle, hres);
			smpscfg_3K.DACDrv.Smpls[i].Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		}

		smpscfg_3K.DACDrv.Cfg.BaseCycles = 297;	// BaseCyles overrides BaseRate and Divider
		smpscfg_3K.DACDrv.Cfg.LoopCycles = 26;
		smpscfg_3K.DACDrv.Cfg.LoopSamples = 2;
		smpscfg_3K.DACDrv.Cfg.RateMode = DACRM_DELAY;
		smpscfg_3K.DACDrv.Cfg.Channels = 1;
		smpscfg_3K.DACDrv.Cfg.VolDiv = 1;

		smpscfg_3K.DACDrv.TblCount = LengthOfArray(DACFiles);
		smpscfg_3K.DACDrv.TblAlloc = smpscfg_3K.DACDrv.TblCount;
		smpscfg_3K.DACDrv.SmplTbl = new DAC_TABLE[smpscfg_3K.DACDrv.TblAlloc];
		ZeroMemory(smpscfg_3K.DACDrv.SmplTbl, smpscfg_3K.DACDrv.TblAlloc * sizeof(DAC_TABLE));

		for (i = 0; i < LengthOfArray(DACFiles); i++)
		{
			smpscfg_3K.DACDrv.SmplTbl[i].Sample = DACFiles[i].resid - IDR_DAC_81;
			smpscfg_3K.DACDrv.SmplTbl[i].Rate = DACFiles[i].rate;
		}


		ZeroMemory(&smpscfg_12.DACDrv, sizeof(smpscfg_12.DACDrv));
		smpscfg_12.DACDrv.SmplCount = (IDR_DAC_87_S2 - IDR_DAC_81_S2) + 1;
		smpscfg_12.DACDrv.SmplAlloc = smpscfg_12.DACDrv.SmplCount;
		smpscfg_12.DACDrv.Smpls = new DAC_SAMPLE[smpscfg_12.DACDrv.SmplAlloc];
		ZeroMemory(smpscfg_12.DACDrv.Smpls, sizeof(DAC_SAMPLE) * smpscfg_12.DACDrv.SmplAlloc);

		for (i = 0; i < smpscfg_12.DACDrv.SmplCount; i++)
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_81_S2 + i), _T("DAC"));
			smpscfg_12.DACDrv.Smpls[i].Compr = COMPR_DPCM;
			smpscfg_12.DACDrv.Smpls[i].DPCMArr = DefDPCMData;
			smpscfg_12.DACDrv.Smpls[i].Size = SizeofResource(moduleHandle, hres);
			smpscfg_12.DACDrv.Smpls[i].Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		}

		smpscfg_12.DACDrv.Cfg.BaseCycles = 288;	// Note: 301 for Sonic 1, 296 for S2B (and 425 for Generic SMPS 68k)
		smpscfg_12.DACDrv.Cfg.LoopCycles = 26;
		smpscfg_12.DACDrv.Cfg.LoopSamples = 2;
		smpscfg_12.DACDrv.Cfg.RateMode = DACRM_DELAY;
		smpscfg_12.DACDrv.Cfg.Channels = 1;
		smpscfg_12.DACDrv.Cfg.VolDiv = 1;

		smpscfg_12.DACDrv.TblCount = LengthOfArray(DACFiles_S2);
		smpscfg_12.DACDrv.TblAlloc = smpscfg_12.DACDrv.TblCount;
		smpscfg_12.DACDrv.SmplTbl = new DAC_TABLE[smpscfg_12.DACDrv.TblAlloc];
		ZeroMemory(smpscfg_12.DACDrv.SmplTbl, smpscfg_12.DACDrv.TblAlloc * sizeof(DAC_TABLE));

		for (i = 0; i < LengthOfArray(DACFiles_S2); i++)
		{
			smpscfg_12.DACDrv.SmplTbl[i].Sample = DACFiles_S2[i].resid - IDR_DAC_81_S2;
			smpscfg_12.DACDrv.SmplTbl[i].Rate = DACFiles_S2[i].rate;
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_INSSET), _T("MISC"));
		smpscfg_3K.GlbInsLen = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg_3K.GlbInsData = (UINT8 *)LockResource(LoadResource(moduleHandle, hres));

		UINT16 InsCount = smpscfg_3K.GlbInsLen / smpscfg_3K.InsRegCnt;
		smpscfg_3K.GlbInsLib.InsCount = InsCount;
		smpscfg_3K.GlbInsLib.InsPtrs = NULL;
		if (InsCount)
		{
			smpscfg_3K.GlbInsLib.InsPtrs = new UINT8*[InsCount];
			UINT16 CurPos = 0x0000;
			for (UINT16 CurIns = 0x00; CurIns < InsCount; CurIns ++, CurPos += smpscfg_3K.InsRegCnt)
				smpscfg_3K.GlbInsLib.InsPtrs[CurIns] = &smpscfg_3K.GlbInsData[CurPos];
		}
		smpscfg_3K.GlbInsBase = 0x17D8;

		smpscfg_12.GlbInsLen = 0x0000;
		smpscfg_12.GlbInsData = NULL;
		smpscfg_12.GlbInsLib.InsCount = InsCount;
		smpscfg_12.GlbInsLib.InsPtrs = NULL;

		DAC_Reset();
		StartAudioOutput();
		InitDriver();

		if (EnableSKCHacks)
		{
			timeBeginPeriod(2);

			srand(_time32(NULL));
		}

		return TRUE;
	}

	BOOL load_song(short id, unsigned int bgmmode)
	{
		if (trackMIDI)
			MIDIFallbackClass->stop_song();
		else
		{
			ThreadPauseConfrm = false;
			PauseThread = true;
			while(! ThreadPauseConfrm)
				Sleep(1);
		}
		int newid = id;
		if (EnableSKCHacks)
		{
			--newid;
			switch (newid)
			{
			case MusicID_LavaReef2:
				if (Current_zone_and_act == hidden_palace_zone
					|| Current_zone_and_act == hidden_palace_shrine)
					newid = MusicID_HiddenPalace;
				break;
			case MusicID_SkySanctuary:
				if (Current_zone_and_act == ending || Ending_running_flag)
					newid = MusicID_Ending;
				break;
			case MusicID_Continue:
				if (Game_mode == GameModeID_BlueSpheresTitle)
					newid = MusicID_BlueSphereTitle;
				else if (Game_mode == GameModeID_TimeAttackRecords)
					newid = MusicID_TimeAttackRecords;
				break;
			case MusicID_ActClear:
				if (Game_mode == GameModeID_SpecialStageResults)
					newid = MusicID_SpecialStageResult;
				else if (Game_mode == GameModeID_BlueSpheresResults)
					newid = MusicID_BlueSphereResult;
				break;
			case MusicID_S3Invincibility:
			case MusicID_SKInvincibility:
				if (Game_mode == GameModeID_BlueSpheresDifficulty)
					newid = MusicID_BlueSphereDifficulty;
				else if (Super_Sonic_Knux_flag || Super_Tails_flag)
					newid = MusicID_SuperSonic;
				break;
			case MusicID_LevelSelect:
				if (Game_mode == GameModeID_SaveScreen)
					newid = MusicID_DataSelect;
				break;
			}
			short *settings = trackSettings;
			if (!GameSelection)
			{
				if (settings[newid] == MusicID_ByCharacter)
					if (Player_mode == 3)
						settings = skTrackSettings;
					else
						settings = s3TrackSettings;
				else if (!GameSelection && settings[newid] == MusicID_ByZone)
				{
					unsigned char level = Current_zone_and_act >> 8;
					switch (level)
					{
					case gumball_bonus:
					case glowing_spheres_bonus:
					case slot_bonus:
						level = Saved_zone_and_act >> 8;
						break;
					}
					if (level == flying_battery_zone || level >= mushroom_hill_zone)
						settings = skTrackSettings;
					else
						settings = s3TrackSettings;
				}
			}
			switch (newid)
			{
			case MusicID_SuperSonic:
			case MusicID_DataSelect:
			case MusicID_SpecialStageResult:
			case MusicID_BlueSphereResult:
				if (settings[newid] == MusicID_Default)
					newid = id - 1;
				break;
			}
			short set = settings[newid];
			if (MIDIFallbackClass && set == MusicID_MIDI)
			{
				trackMIDI = true;
				return MIDIFallbackClass->load_song(id, bgmmode);
			}
			else if (set == MusicID_Random)
			{
				const tracknameoptions *opt = &TrackOptions[newid];
				newid = opt->options[rand() % opt->optioncount].id;
			}
			else if (set != MusicID_Default)
				newid = set;
		}
		trackMIDI = false;
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
				LoadRegisterList(cursmpscfg, LengthOfArray(INSOPS_DEFAULT), INSOPS_DEFAULT);
				cursmpscfg->TempoMode = TEMPO_TIMEOUT;
				break;
			case TrackMode_S2B:
				cursmpscfg->PtrFmt = PTRFMT_Z80;
				cursmpscfg->InsMode = INSMODE_HW;
				LoadRegisterList(cursmpscfg, LengthOfArray(INSOPS_HARDWARE), INSOPS_HARDWARE);
				cursmpscfg->TempoMode = TEMPO_TIMEOUT;
				break;
			case TrackMode_S2:
				cursmpscfg->PtrFmt = PTRFMT_Z80;
				cursmpscfg->InsMode = INSMODE_HW;
				LoadRegisterList(cursmpscfg, LengthOfArray(INSOPS_HARDWARE), INSOPS_HARDWARE);
				cursmpscfg->TempoMode = TEMPO_OVERFLOW2;
				break;
			}
			break;
		default:
			DRUM_LIB* drumLib;
			
			cursmpscfg = &smpscfg_3K;
			drumLib = &cursmpscfg->DrumLib;
			if (song->mode == TrackMode_S3)
			{
				cursmpscfg->VolEnvs = VolEnvs_S3;
				cursmpscfg->DACDrv.SmplTbl[0xB2-0x81].Sample = IDR_DAC_B2_S3 - IDR_DAC_81;
				cursmpscfg->DACDrv.SmplTbl[0xB3-0x81].Sample = IDR_DAC_B2_S3 - IDR_DAC_81;
			}
			else
			{
				cursmpscfg->VolEnvs = VolEnvs_SK;
				cursmpscfg->DACDrv.SmplTbl[0xB2-0x81].Sample = IDR_DAC_B2 - IDR_DAC_81;
				cursmpscfg->DACDrv.SmplTbl[0xB3-0x81].Sample = IDR_DAC_B2 - IDR_DAC_81;
			}

			UINT8 i;
			if (! bgmmode && fmdrum_on)
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
					drumLib->DrumData[0x9F-0x80].Type = DRMTYPE_FM;
					drumLib->DrumData[0x9F-0x80].DrumID = 0x86 - 0x81;
					drumLib->DrumData[0xA0-0x80].Type = DRMTYPE_DAC;
					drumLib->DrumData[0xA0-0x80].DrumID = S3D_ID_BASE + (0xA0-0x9F);
				}
			}
			else if (song->mode == TrackMode_S3D)
			{
				// Sonic 3D DAC drums
				for (i = 1; i < 0x1F; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = i - 1;	// set DAC sample
				}
				for (; i <= 0x20; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = S3D_ID_BASE + (i - 0x1F);	// S3D sounds 9F/A0 are C5/C6 here.
				}
				for (; i < drumLib->DrumCount; i++)
					drumLib->DrumData[i].Type = DRMTYPE_NONE;
			}
			else
			{
				// Sonic 3/K DAC drums
				for (i = 1; i < drumLib->DrumCount; i++)
				{
					drumLib->DrumData[i].Type = DRMTYPE_DAC;
					drumLib->DrumData[i].DrumID = i - 1;	// set DAC sample
				}
			}
			break;
		}	// end switch (song->mode)

		cursmpscfg->SeqBase = song->base;
		cursmpscfg->SeqLength = song->length;
		cursmpscfg->SeqData = song->data;
		PreparseSMPSFile(cursmpscfg);
		return TRUE;
	}

	BOOL play_song()
	{
		if (trackMIDI)
			return MIDIFallbackClass->play_song();
		if (cursmpscfg == NULL)
			return TRUE;
		
		ThreadPauseConfrm = false;
		PauseThread = true;
		while(! ThreadPauseConfrm)
			Sleep(1);
		PauseStream(false);

		SmplsPerFrame = SampleRate / FrameDivider;
		PlayMusic(cursmpscfg);
		PauseThread = false;
		return TRUE;
	}

	BOOL stop_song()
	{
		if (EnableSKCHacks)
		{
			if (reg_d0 == 0xFF)
				return PlaySound(MAKEINTRESOURCE(IDR_WAVE_SEGA), moduleHandle, SND_RESOURCE | SND_ASYNC);
			else if (reg_d0 == 0xFE)
				return PlaySound(NULL, NULL, SND_ASYNC);
		}
		if (trackMIDI)
			return MIDIFallbackClass->stop_song();
		if (EnableSKCHacks && reg_d0 == 0xE1)
			FadeOutMusic();
		else
		{
			ThreadPauseConfrm = false;
			PauseThread = true;
			while(! ThreadPauseConfrm)
				Sleep(1);
			StopAllSound();
		}
		return TRUE;
	}

	BOOL pause_song()
	{
		if (trackMIDI)
			return MIDIFallbackClass->pause_song();
		PauseStream(true);
		return TRUE;
	}

	BOOL unpause_song()
	{
		if (trackMIDI)
			return MIDIFallbackClass->unpause_song();
		PauseStream(false);
		return TRUE;
	}

	BOOL set_song_tempo(unsigned int pct)
	{
		if (trackMIDI)
			return MIDIFallbackClass->set_song_tempo(pct);
		SmplsPerFrame = (SampleRate * pct) / (FrameDivider * 100);
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
void (*SongStoppedCallback)() = NULL;

extern "C"
{
	__declspec(dllexport) SMPSInterfaceClass *GetMidiInterface()
	{
		return &midiInterface;
	}

	__declspec(dllexport) BOOL InitializeDriver()
	{
		EnableSKCHacks = false;
		return midiInterface.initialize(NULL);
	}

	__declspec(dllexport) void RegisterSongStoppedCallback(void (*callback)())
	{
		SongStoppedCallback = callback;
	}

	__declspec(dllexport) BOOL PlaySong(short song)
	{
		if (!midiInterface.load_song(song, 0))
			return FALSE;
		return midiInterface.play_song();
	}

	__declspec(dllexport) BOOL StopSong()
	{
		return midiInterface.stop_song();
	}

	__declspec(dllexport) void FadeOutSong()
	{
		FadeOutMusic();
	}

	__declspec(dllexport) BOOL PauseSong()
	{
		return midiInterface.pause_song();
	}

	__declspec(dllexport) BOOL ResumeSong()
	{
		return midiInterface.unpause_song();
	}

	__declspec(dllexport) BOOL SetSongTempo(unsigned int pct)
	{
		return midiInterface.set_song_tempo(pct);
	}

	__declspec(dllexport) const char **GetCustomSongs(unsigned int &count)
	{
		count = customsongs.size();
		return customsongs.data();
	}

	void NotifySongStopped()
	{
		if (EnableSKCHacks)
			PostMessageA(gameWindow, 0x464u, 0, 0);
		else if (SongStoppedCallback)
			SongStoppedCallback();
	}
}