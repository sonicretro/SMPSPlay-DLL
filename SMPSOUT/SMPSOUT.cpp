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
#include <unordered_map>
#define INISUPPORT 1
#endif
#include <fstream>
#include <ctime>
#include "resource.h"
using namespace std;

#ifdef INISUPPORT
typedef unordered_map<string, string> IniGroup;
struct IniGroupStr { IniGroup Element; };
typedef unordered_map<string, IniGroupStr> IniDictionary;

IniDictionary LoadINI(istream &textfile)
{
	IniDictionary result = IniDictionary();
	result[""] = IniGroupStr();
	IniGroupStr *curent = &result[""];
	while (textfile.good())
	{
		string line;
		getline(textfile, line);
		string sb = string();
		sb.reserve(line.length());
		bool startswithbracket = false;
		int firstequals = -1;
		int endbracket = -1;
		for (int c = 0; c < (int)line.length(); c++)
			switch (line[c])
		{
			case '\\': // escape character
				if (c + 1 == line.length())
					goto appendchar;
				c++;
				switch (line[c])
				{
				case 'n': // line feed
					sb += '\n';
					break;
				case 'r': // carriage return
					sb += '\r';
					break;
				default: // literal character
					goto appendchar;
				}
				break;
			case '=':
				if (firstequals == -1)
					firstequals = sb.length();
				goto appendchar;
			case '[':
				if (c == 0)
					startswithbracket = true;
				goto appendchar;
			case ']':
				endbracket = sb.length();
				goto appendchar;
			case ';': // comment character, stop processing this line
				c = line.length();
				break;
			default:
appendchar:
				sb += line[c];
				break;
		}
		line = sb;
		if (startswithbracket && endbracket != -1)
		{
			line = line.substr(1, endbracket - 1);
			result[line] = IniGroupStr();
			curent = &result[line];
		}
		else if (!line.empty())
		{
			string key;
			string value = "";
			if (firstequals > -1)
			{
				key = line.substr(0, firstequals);
				size_t endpos = key.find_last_not_of(" \t");
				if (string::npos != endpos)
					key = key.substr(0, endpos + 1);	// trim trailing spaces, see Stack Overflow, Question 216823

				value = line.substr(firstequals + 1);
				size_t startpos  = value.find_first_not_of(" \t");
				if (string::npos != endpos)
					value = value.substr(startpos);	// trim leading spaces, see Stack Overflow, Question 216823
			}
			else
				key = line;
			(*curent).Element[key] = value;
		}
	}
	return result;
}

IniDictionary LoadINI(const string &filename)
{
	ifstream str(filename);
	IniDictionary dict = LoadINI(str);
	str.close();
	return dict;
}

IniDictionary LoadINI(const wstring &filename)
{
	ifstream str(filename);
	IniDictionary dict = LoadINI(str);
	str.close();
	return dict;
}
#endif

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

enum MusicID {
	MusicID_MIDI = -3,
	MusicID_Random,
	MusicID_Default,
	MusicID_S3Title,
	MusicID_AngelIsland1,
	MusicID_AngelIsland2,
	MusicID_Hydrocity1,
	MusicID_Hydrocity2,
	MusicID_MarbleGarden1,
	MusicID_MarbleGarden2,
	MusicID_CarnivalNight1,
	MusicID_CarnivalNight2,
	MusicID_FlyingBattery1,
	MusicID_FlyingBattery2,
	MusicID_IceCap1,
	MusicID_IceCap2,
	MusicID_LaunchBase1,
	MusicID_LaunchBase2,
	MusicID_MushroomHill1,
	MusicID_MushroomHill2,
	MusicID_Sandopolis1,
	MusicID_Sandopolis2,
	MusicID_LavaReef1,
	MusicID_LavaReef2,
	MusicID_SkySanctuary,
	MusicID_DeathEgg1,
	MusicID_DeathEgg2,
	MusicID_Midboss,
	MusicID_SKMidboss = MusicID_Midboss,
	MusicID_Boss,
	MusicID_Doomsday,
	MusicID_MagneticOrbs,
	MusicID_SpecialStage,
	MusicID_SlotMachine,
	MusicID_GumballMachine,
	MusicID_S3Knuckles,
	MusicID_AzureLake,
	MusicID_BalloonPark,
	MusicID_DesertPalace,
	MusicID_ChromeGadget,
	MusicID_EndlessMine,
	MusicID_GameOver,
	MusicID_Continue,
	MusicID_ActClear,
	MusicID_S31Up,
	MusicID_ChaosEmerald,
	MusicID_S3Invincibility,
	MusicID_CompetitionMenu,
	MusicID_Unused,
	MusicID_S3Midboss = MusicID_Unused,
	MusicID_LevelSelect,
	MusicID_FinalBoss,
	MusicID_Drowning,
	MusicID_S3AllClear,
	MusicID_S3Credits,
	MusicID_SKKnuckles,
	MusicID_SKTitle,
	MusicID_SK1Up,
	MusicID_SKInvincibility,
	MusicID_SKAllClear,
	MusicID_SKCredits,
	MusicID_S3CCredits,
	MusicID_HiddenPalace = MusicID_S3CCredits,
	MusicID_S3Continue,
	// begin S&KC tracks
	MusicID_CarnivalNight1PC,
	MusicID_CarnivalNight2PC,
	MusicID_IceCap1PC,
	MusicID_IceCap2PC,
	MusicID_LaunchBase1PC,
	MusicID_LaunchBase2PC,
	MusicID_KnucklesPC,
	MusicID_CompetitionMenuPC,
	MusicID_UnusedPC,
	MusicID_CreditsPC,
	MusicID_S3InvincibilityPC,
	// end S&KC tracks
	MusicID_SKTitle0525,
	MusicID_SKAllClear0525,
	MusicID_SKCredits0525,
	MusicID_GreenGrove1,
	MusicID_GreenGrove2,
	MusicID_RustyRuin1,
	MusicID_RustyRuin2,
	MusicID_VolcanoValley2,
	MusicID_VolcanoValley1,
	MusicID_SpringStadium1,
	MusicID_SpringStadium2,
	MusicID_DiamondDust1,
	MusicID_DiamondDust2,
	MusicID_GeneGadget1,
	MusicID_GeneGadget2,
	MusicID_PanicPuppet2,
	MusicID_FinalFight,
	MusicID_S3DEnding,
	MusicID_S3DSpecialStage,
	MusicID_PanicPuppet1,
	MusicID_S3DBoss2,
	MusicID_S3DBoss1,
	MusicID_S3DIntro,
	MusicID_S3DCredits,
	MusicID_S3DInvincibility,
	MusicID_S3DMenu,
	MusicID_S4E1Boss
};

#define TrackCount MusicID_HiddenPalace+1

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

enum EntryMode {
	TrackMode_SK,
	TrackMode_S3,
	TrackMode_S3D
};

struct musicentry { unsigned short base; unsigned char mode; };

musicentry MusicFiles[] = {
	{ 0xE18F, TrackMode_S3 }, // 1
	{ 0x8000, TrackMode_S3 }, // 2
	{ 0x9B6D, TrackMode_S3 }, // 3
	{ 0xB0BC, TrackMode_S3 }, // 4
	{ 0xC0C6, TrackMode_S3 }, // 5
	{ 0xD364, TrackMode_S3 }, // 6
	{ 0xD97B, TrackMode_S3 }, // 7
	{ 0xE48F, TrackMode_S3 }, // 8
	{ 0xDDA9, TrackMode_S3 }, // 9
	{ 0x8000, TrackMode_SK }, // 10
	{ 0x8597, TrackMode_SK }, // 11
	{ 0x86AA, TrackMode_S3 }, // 12
	{ 0x8000, TrackMode_S3 }, // 13
	{ 0x90CF, TrackMode_S3 }, // 14
	{ 0x8DC8, TrackMode_S3 }, // 15
	{ 0x8AFE, TrackMode_SK }, // 16
	{ 0x9106, TrackMode_SK }, // 17
	{ 0x9688, TrackMode_SK }, // 18
	{ 0x9CF2, TrackMode_SK }, // 19
	{ 0xA2E5, TrackMode_SK }, // 20
	{ 0xACF3, TrackMode_SK }, // 21
	{ 0xBE80, TrackMode_SK }, // 22
	{ 0xC2B4, TrackMode_SK }, // 23
	{ 0xC79F, TrackMode_SK }, // 24
	{ 0xCBB1, TrackMode_SK }, // 25
	{ 0xCEE1, TrackMode_SK }, // 26
	{ 0xD3DD, TrackMode_SK }, // 27
	{ 0xDCC0, TrackMode_SK }, // 28
	{ 0xE223, TrackMode_SK }, // 29
	{ 0xEABB, TrackMode_SK }, // 30
	{ 0x8AE8, TrackMode_S3 }, // 31
	{ 0x97FD, TrackMode_S3 }, // 32
	{ 0x99F7, TrackMode_S3 }, // 33
	{ 0xA4FD, TrackMode_S3 }, // 34
	{ 0xB0EC, TrackMode_S3 }, // 35
	{ 0xC324, TrackMode_S3 }, // 36
	{ 0xDA47, TrackMode_S3 }, // 37
	{ 0xDD4B, TrackMode_SK }, // 38
	{ 0xDFA6, TrackMode_SK }, // 39
	{ 0xE3C0, TrackMode_SK }, // 40
	{ 0xF095, TrackMode_S3 }, // 41
	{ 0xFE75, TrackMode_SK }, // 42
	{ 0xF364, TrackMode_S3 }, // 43
	{ 0xF5E4, TrackMode_S3 }, // 44
	{ 0xF1A0, TrackMode_S3 }, // 45
	{ 0xE7AF, TrackMode_SK }, // 46
	{ 0xF74C, TrackMode_SK }, // 47
	{ 0xFABE, TrackMode_SK }, // 48
	{ 0xFBFE, TrackMode_S3 }, // 49
	{ 0xE587, TrackMode_S3 }, // 50
	{ 0xF5A3, TrackMode_SK }, // 51
	{ 0xF88E, TrackMode_SK }, // 52
	{ 0xFD4B, TrackMode_SK }, // 53
	{ 0xE574, TrackMode_SK }, // 54
	{ 0xFCDE, TrackMode_SK }, // 55
	{ 0xC104, TrackMode_SK }, // 56
	{ 0xE33A, TrackMode_S3 }, // 57 (S3C uses S3 PSGs and DACs)
	{ 0xEC7B, TrackMode_S3 }, // 58
	// begin S&KC tracks
	{ 0x8000, TrackMode_SK }, // 59
	{ 0x8000, TrackMode_SK }, // 60
	{ 0x8000, TrackMode_SK }, // 61
	{ 0x8000, TrackMode_SK }, // 62
	{ 0x8000, TrackMode_SK }, // 63
	{ 0x8000, TrackMode_SK }, // 64
	{ 0x8000, TrackMode_SK }, // 65
	{ 0x8000, TrackMode_SK }, // 66
	{ 0x8000, TrackMode_SK }, // 67
	{ 0x8000, TrackMode_SK }, // 68
	{ 0xF364, TrackMode_S3 }, // 69
	// end S&KC tracks
	{ 0x8000, TrackMode_S3 }, // 70 (S&K Beta 0525 uses S3 PSGs and DACs, too)
	{ 0xC294, TrackMode_S3 }, // 71
	{ 0x84BD, TrackMode_S3 }, // 72
	{ 0x8024, TrackMode_S3D }, // 73
	{ 0x918F, TrackMode_S3D }, // 74
	{ 0xA8A0, TrackMode_S3D }, // 75
	{ 0xB9E7, TrackMode_S3D }, // 76
	{ 0xC795, TrackMode_S3D }, // 77
	{ 0xD04A, TrackMode_S3D }, // 78
	{ 0xD739, TrackMode_S3D }, // 79
	{ 0xE372, TrackMode_S3D }, // 80
	{ 0xF091, TrackMode_S3D }, // 81
	{ 0x8024, TrackMode_S3D }, // 82
	{ 0x8BDE, TrackMode_S3D }, // 83
	{ 0x96BD, TrackMode_S3D }, // 84
	{ 0xA0A8, TrackMode_S3D }, // 85
	{ 0xA8E0, TrackMode_S3D }, // 86
	{ 0x96B3, TrackMode_S3D }, // 87
	{ 0xBB55, TrackMode_S3D }, // 88
	{ 0xC68F, TrackMode_S3D }, // 89
	{ 0xD012, TrackMode_S3D }, // 90
	{ 0xD620, TrackMode_S3D }, // 91
	{ 0xDD98, TrackMode_S3D }, // 92
	{ 0xE1D9, TrackMode_S3D }, // 93
	{ 0x8D55, TrackMode_S3D }, // 94
	{ 0x9127, TrackMode_S3D }, // 95
	{ 0xCEC7, TrackMode_S3D }, // 96
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
0x40, 0x48, 0x44, 0x4C,
0x00};	// The 0 is required by the SMPS routines to terminate the array.

enum DriverMode {
	SmpsDrv_S3K
};


#if ! defined(_MSC_VER) || _MSC_VER >= 1600
template <typename T, size_t N>
inline size_t LengthOfArray(const T(&)[N])
{
	return N;
}
#else
#define LengthOfArray(x)	(sizeof(x) / sizeof((x)[0]))
#endif

struct trackoption { const char *text; char id; };

const trackoption TitleScreenTrackOptions[] = {
	{ "S3", MusicID_S3Title },
	{ "S&K", MusicID_SKTitle },
	{ "S&K0525", MusicID_SKTitle0525 },
	{ "S3D", MusicID_S3DIntro }
};

const trackoption AngelIsland1TrackOptions[] = {
	{ "S3K", MusicID_AngelIsland1 },
	{ "GreenGrove", MusicID_GreenGrove1 }
};

const trackoption AngelIsland2TrackOptions[] = {
	{ "S3K", MusicID_AngelIsland2 },
	{ "GreenGrove", MusicID_GreenGrove2 }
};

const trackoption Hydrocity1TrackOptions[] = {
	{ "S3K", MusicID_Hydrocity1 },
	{ "RustyRuin", MusicID_RustyRuin1 },
};

const trackoption Hydrocity2TrackOptions[] = {
	{ "S3K", MusicID_Hydrocity2 },
	{ "RustyRuin", MusicID_RustyRuin2 }
};

const trackoption CarnivalNight1TrackOptions[] = {
	{ "MD", MusicID_CarnivalNight1 },
#ifdef SKCMUSIC
	{ "PC", MusicID_CarnivalNight1PC },
#endif
	{ "SpringStadium", MusicID_SpringStadium1 }
};

const trackoption CarnivalNight2TrackOptions[] = {
	{ "MD", MusicID_CarnivalNight2 },
#ifdef SKCMUSIC
	{ "PC", MusicID_CarnivalNight2PC },
#endif
	{ "SpringStadium", MusicID_SpringStadium2 }
};

const trackoption FlyingBattery1TrackOptions[] = {
	{ "S3K", MusicID_FlyingBattery1 },
	{ "PanicPuppet", MusicID_PanicPuppet1 }
};

const trackoption FlyingBattery2TrackOptions[] = {
	{ "S3K", MusicID_FlyingBattery2 },
	{ "PanicPuppet", MusicID_PanicPuppet2 }
};

const trackoption IceCap1TrackOptions[] = {
	{ "MD", MusicID_IceCap1 },
#ifdef SKCMUSIC
	{ "PC", MusicID_IceCap1PC },
#endif
	{ "DiamondDust", MusicID_DiamondDust1 }
};

const trackoption IceCap2TrackOptions[] = {
	{ "MD", MusicID_IceCap2 },
#ifdef SKCMUSIC
	{ "PC", MusicID_IceCap2PC },
#endif
	{ "DiamondDust", MusicID_DiamondDust2 }
};

const trackoption LaunchBase1TrackOptions[] = {
	{ "MD", MusicID_LaunchBase1 },
#ifdef SKCMUSIC
	{ "PC", MusicID_LaunchBase1PC },
#endif
	{ "GeneGadget", MusicID_GeneGadget1 },
	{ "PanicPuppet", MusicID_PanicPuppet1 }
};

const trackoption LaunchBase2TrackOptions[] = {
	{ "MD", MusicID_LaunchBase2 },
#ifdef SKCMUSIC
	{ "PC", MusicID_LaunchBase2PC },
#endif
	{ "GeneGadget", MusicID_GeneGadget2 },
	{ "PanicPuppet", MusicID_PanicPuppet2 }
};

const trackoption MushroomHill1TrackOptions[] = {
	{ "S3K", MusicID_MushroomHill1 },
	{ "GreenGrove", MusicID_GreenGrove1 }
};

const trackoption MushroomHill2TrackOptions[] = {
	{ "S3K", MusicID_MushroomHill2 },
	{ "GreenGrove", MusicID_GreenGrove2 }
};

const trackoption LavaReef1TrackOptions[] = {
	{ "S3K", MusicID_LavaReef1 },
	{ "VolcanoValley", MusicID_VolcanoValley1 }
};

const trackoption LavaReef2TrackOptions[] = {
	{ "S3K", MusicID_LavaReef2 },
	{ "VolcanoValley", MusicID_VolcanoValley2 }
};

const trackoption DeathEgg1TrackOptions[] = {
	{ "S3K", MusicID_DeathEgg1 },
	{ "GeneGadget", MusicID_GeneGadget1 },
	{ "PanicPuppet", MusicID_PanicPuppet1 }
};

const trackoption DeathEgg2TrackOptions[] = {
	{ "S3K", MusicID_DeathEgg2 },
	{ "GeneGadget", MusicID_GeneGadget2 },
	{ "PanicPuppet", MusicID_PanicPuppet2 }
};

const trackoption MidbossTrackOptions[] = {
	{ "S3", MusicID_S3Midboss },
	{ "S&K", MusicID_SKMidboss },
	{ "S3D1", MusicID_S3DBoss1 },
	{ "S3D2", MusicID_S3DBoss2 },
	{ "S4E1", MusicID_S4E1Boss }
};

const trackoption BossTrackOptions[] = {
	{ "S3K", MusicID_Boss },
	{ "S3D1", MusicID_S3DBoss1 },
	{ "S3D2", MusicID_S3DBoss2 },
	{ "S4E1", MusicID_S4E1Boss }
};

const trackoption DoomsdayTrackOptions[] = {
	{ "S3K", MusicID_Doomsday },
	{ "S3D1", MusicID_S3DBoss1 },
	{ "S3D2", MusicID_S3DBoss2 },
	{ "FinalFight", MusicID_FinalFight },
	{ "S4E1", MusicID_S4E1Boss }
};

const trackoption SpecialStageTrackOptions[] = {
	{ "S3K", MusicID_SpecialStage },
	{ "S3D", MusicID_S3DSpecialStage }
};

const trackoption KnucklesTrackOptions[] = {
	{ "S3", MusicID_S3Knuckles },
	{ "S&K", MusicID_SKKnuckles },
#ifdef SKCMUSIC
	{ "PC", MusicID_KnucklesPC }
#endif
};

const trackoption ContinueTrackOptions[] = { { "S3", MusicID_S3Continue }, { "S&K", MusicID_Continue } };

const trackoption OneUpTrackOptions[] = { { "S3", MusicID_S31Up }, { "S&K", MusicID_SK1Up } };

const trackoption InvincibilityTrackOptions[] = {
	{ "S3", MusicID_S3Invincibility },
	{ "S&K", MusicID_SKInvincibility },
#ifdef SKCMUSIC
	{ "S3PC", MusicID_S3InvincibilityPC },
	{ "PC", MusicID_UnusedPC },
#endif
	{ "S3D", MusicID_S3DInvincibility }
};

const trackoption CompetitionMenuTrackOptions[] = {
	{ "MD", MusicID_CompetitionMenu },
#ifdef SKCMUSIC
	{ "PC", MusicID_CompetitionMenuPC },
#endif
	{ "S3D", MusicID_S3DMenu }
};

const trackoption LevelSelectTrackOptions[] = {
	{ "S3K", MusicID_LevelSelect },
	{ "S3D", MusicID_S3DMenu },
	{ "PanicPuppet1", MusicID_PanicPuppet1 }
};

const trackoption FinalBossTrackOptions[] = {
	{ "S3K", MusicID_FinalBoss },
	{ "S3D1", MusicID_S3DBoss1 },
	{ "S3D2", MusicID_S3DBoss2 },
	{ "FinalFight", MusicID_FinalFight },
	{ "S4E1", MusicID_S4E1Boss }
};

const trackoption AllClearTrackOptions[] = {
	{ "S3", MusicID_S3AllClear },
	{ "S&K", MusicID_SKAllClear },
	{ "S&K0525", MusicID_SKAllClear0525 },
	{ "S3D", MusicID_S3DEnding }
};

const trackoption CreditsTrackOptions[] = {
	{ "S3", MusicID_S3Credits },
	{ "S&K", MusicID_SKCredits },
	{ "S3C", MusicID_S3CCredits },
	{ "S&K0525", MusicID_SKCredits0525 },
#ifdef SKCMUSIC
	{ "PC", MusicID_CreditsPC },
#endif
	{ "S3D", MusicID_S3DCredits }
};

struct tracknameoptions { const char *name; const trackoption *options; int optioncount; };

#define arrayptrandlength(x) x, LengthOfArray(x)

#define trackoptdef(x) { #x, arrayptrandlength(x##Options) }

const tracknameoptions TrackOptions[TrackCount] = {
	trackoptdef(TitleScreenTrack),
	trackoptdef(AngelIsland1Track),
	trackoptdef(AngelIsland2Track),
	trackoptdef(Hydrocity1Track),
	trackoptdef(Hydrocity2Track),
	{ "MarbleGarden1Track" },
	{ "MarbleGarden2Track" },
	trackoptdef(CarnivalNight1Track),
	trackoptdef(CarnivalNight2Track),
	trackoptdef(FlyingBattery1Track),
	trackoptdef(FlyingBattery2Track),
	trackoptdef(IceCap1Track),
	trackoptdef(IceCap2Track),
	trackoptdef(LaunchBase1Track),
	trackoptdef(LaunchBase2Track),
	trackoptdef(MushroomHill1Track),
	trackoptdef(MushroomHill2Track),
	{ "Sandopolis1Track" },
	{ "Sandopolis2Track" },
	trackoptdef(LavaReef1Track),
	trackoptdef(LavaReef2Track),
	{ "SkySanctuaryTrack" },
	trackoptdef(DeathEgg1Track),
	trackoptdef(DeathEgg2Track),
	trackoptdef(MidbossTrack),
	trackoptdef(BossTrack),
	trackoptdef(DoomsdayTrack),
	{ "MagneticOrbsTrack" },
	trackoptdef(SpecialStageTrack),
	{ "SlotMachineTrack" },
	{ "GumballMachineTrack" },
	trackoptdef(KnucklesTrack),
	{ "AzureLakeTrack" },
	{ "BalloonParkTrack" },
	{ "DesertPalaceTrack" },
	{ "ChromeGadgetTrack" },
	{ "EndlessMineTrack" },
	{ "GameOverTrack" },
	trackoptdef(ContinueTrack),
	{ "ActClearTrack" },
	{ "1UpTrack", arrayptrandlength(OneUpTrackOptions) },
	{ "ChaosEmeraldTrack" },
	trackoptdef(InvincibilityTrack),
	trackoptdef(CompetitionMenuTrack),
	{ "UnusedTrack" },
	trackoptdef(LevelSelectTrack),
	trackoptdef(FinalBossTrack),
	{ "DrowningTrack" },
	trackoptdef(AllClearTrack),
	trackoptdef(CreditsTrack),
	trackoptdef(KnucklesTrack),
	trackoptdef(TitleScreenTrack),
	{ "1UpTrack", arrayptrandlength(OneUpTrackOptions) },
	trackoptdef(InvincibilityTrack),
	trackoptdef(AllClearTrack),
	trackoptdef(CreditsTrack),
	{ "HiddenPalaceTrack" }
};

unsigned int &GameSelection = *(unsigned int *)0x831188;
unsigned char &reg_d0 = *(unsigned char *)0x8549A4;
unsigned short &Current_zone_and_act = *(unsigned short *)0x8FFFE10;

#define hidden_palace_zone 0x1601
#define hidden_palace_shrine 0x1701

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

bool EnableSKCHacks = true;

class SMPSInterfaceClass : MidiInterfaceClass
{
	SMPS_CFG smpscfg;
	ENV_LIB VolEnvs_S3;
	ENV_LIB VolEnvs_SK;
	bool fmdrum_on;
	char trackSettings[3][TrackCount];
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

			SmpsCfg->FMFreqs = (UINT16*)DEF_FMFREQ_VAL;
			SmpsCfg->FMFreqCnt = LengthOfArray(DEF_FMFREQ_VAL);
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
			SmpsCfg->CmdMetaList.FlagBase = 0;
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
	void ReadSettings(const IniGroup &settings, char *trackSettings)
	{
		for (auto iter = settings.cbegin(); iter != settings.cend(); iter++)
		{
			if (iter->first == "FMDrums")
			{
				const char* cstr = iter->second.c_str();
				if (! _stricmp(cstr, "True"))
					fmdrum_on = true;
				else if (! _stricmp(cstr, "False"))
					fmdrum_on = false;
				else
					fmdrum_on = atoi(cstr) ? true : false;
				continue;
			}
			else
				for (int i = 0; i < TrackCount; i++)
					if (iter->first == TrackOptions[i].name)
					{
						if (iter->second == "MIDI")
							trackSettings[i] = MusicID_MIDI;
						else if (TrackOptions[i].optioncount < 2)
							continue;
						else if (iter->second == "Random")
							trackSettings[i] = MusicID_Random;
						else
							for (int j = 0; j < TrackOptions[i].optioncount; j++)
								if (iter->second == TrackOptions[i].options[j].text)
								{
									trackSettings[i] = TrackOptions[i].options[j].id;
									break; // can't break outer loop due to S3/S&K copies of tracks
								}
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

		ZeroMemory(&smpscfg, sizeof(smpscfg));
		fmdrum_on = false;

		if (EnableSKCHacks)
		{
			HMODULE midimodule = LoadLibrary(_T("MIDIOUTY.DLL"));
			if (midimodule)
			{
				MIDIFallbackClass = ((MidiInterfaceClass *(*)())GetProcAddress(midimodule, "GetMidiInterface"))();
				MIDIFallbackClass->initialize(hwnd);
			}

			gameWindow = hwnd;

			char masterSettings[TrackCount];
			memset(&masterSettings, MusicID_Default, TrackCount);

#ifdef INISUPPORT
			IniDictionary settings = LoadINI(_T("SMPSOUT.ini"));

			auto iter = settings.find("");
			if (iter != settings.cend())
				ReadSettings(iter->second.Element, masterSettings);

			if (masterSettings[MusicID_HiddenPalace] == MusicID_Default)
				masterSettings[MusicID_HiddenPalace] = MusicID_LavaReef2;

			memmove(trackSettings[0], masterSettings, TrackCount);
			memmove(trackSettings[1], masterSettings, TrackCount);
			memmove(trackSettings[2], masterSettings, TrackCount);

			iter = settings.find("S3K");
			if (iter != settings.cend())
				ReadSettings(iter->second.Element, trackSettings[0]);

			iter = settings.find("S&K");
			if (iter != settings.cend())
				ReadSettings(iter->second.Element, trackSettings[1]);

			iter = settings.find("S3");
			if (iter != settings.cend())
				ReadSettings(iter->second.Element, trackSettings[2]);
#else
			masterSettings[MusicID_HiddenPalace] = MusicID_LavaReef2;
			memmove(trackSettings[0], masterSettings, TrackCount);
			memmove(trackSettings[1], masterSettings, TrackCount);
			memmove(trackSettings[2], masterSettings, TrackCount);
			fmdrum_on = true;
#endif
			if (trackSettings[2][MusicID_Midboss] == MusicID_Default)
				trackSettings[2][MusicID_Midboss] = MusicID_S3Midboss;
			if (trackSettings[2][MusicID_Continue] == MusicID_Default)
				trackSettings[2][MusicID_Continue] = MusicID_S3Continue;
		}
		LoadSettings(SmpsDrv_S3K, &smpscfg);

		ZeroMemory(&smpscfg.DrumLib, sizeof(smpscfg.DrumLib));
		smpscfg.DrumLib.DrumCount = 0x60;
		smpscfg.DrumLib.DrumData = new DRUM_DATA[smpscfg.DrumLib.DrumCount];
		ZeroMemory(smpscfg.DrumLib.DrumData, sizeof(DRUM_DATA) * smpscfg.DrumLib.DrumCount);

		smpscfg.DrumLib.DrumData[0].Type = DRMTYPE_NONE;
		for (i = 1; i < smpscfg.DrumLib.DrumCount; i++)
		{
			smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
			smpscfg.DrumLib.DrumData[i].DrumID = i - 1;	// set DAC sample
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_MODULAT), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &smpscfg.ModEnvs);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG_S3), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &VolEnvs_S3);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_PSG), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		GetEnvelopeData(dataSize, dataPtr, &VolEnvs_SK);

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_FM_DRUMS), _T("MISC"));
		dataSize = SizeofResource(moduleHandle, hres);
		dataPtr = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		LoadDrumTracks(dataSize, dataPtr, &smpscfg.FMDrums, 0x01);	// 0x01 - FM drums

		smpscfg.VolEnvs = VolEnvs_SK;

		ZeroMemory(&smpscfg.DACDrv, sizeof(smpscfg.DACDrv));
		smpscfg.DACDrv.SmplCount = (IDR_DAC_A0_S3D - IDR_DAC_81) + 1;	// all DAC sounds including S3's B2
		smpscfg.DACDrv.SmplAlloc = smpscfg.DACDrv.SmplCount;
		smpscfg.DACDrv.Smpls = new DAC_SAMPLE[smpscfg.DACDrv.SmplAlloc];
		ZeroMemory(smpscfg.DACDrv.Smpls, sizeof(DAC_SAMPLE) * smpscfg.DACDrv.SmplAlloc);

		for (i = 0; i < smpscfg.DACDrv.SmplCount; i++)
		{
			hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_DAC_81 + i), _T("DAC"));
			smpscfg.DACDrv.Smpls[i].Compr = COMPR_DPCM;
			smpscfg.DACDrv.Smpls[i].DPCMArr = DefDPCMData;
			smpscfg.DACDrv.Smpls[i].Size = SizeofResource(moduleHandle, hres);
			smpscfg.DACDrv.Smpls[i].Data = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		}

		smpscfg.DACDrv.Cfg.BaseCycles = 297;	// BaseCyles overrides BaseRate and Divider
		smpscfg.DACDrv.Cfg.LoopCycles = 26;
		smpscfg.DACDrv.Cfg.LoopSamples = 2;
		smpscfg.DACDrv.Cfg.RateMode = DACRM_DELAY;
		smpscfg.DACDrv.Cfg.Channels = 1;
		smpscfg.DACDrv.Cfg.VolDiv = 1;

		smpscfg.DACDrv.TblCount = LengthOfArray(DACFiles);
		smpscfg.DACDrv.TblAlloc = smpscfg.DACDrv.TblCount;
		smpscfg.DACDrv.SmplTbl = new DAC_TABLE[smpscfg.DACDrv.TblAlloc];
		ZeroMemory(smpscfg.DACDrv.SmplTbl, smpscfg.DACDrv.TblAlloc * sizeof(DAC_TABLE));

		for (i = 0; i < LengthOfArray(DACFiles); i++)
		{
			smpscfg.DACDrv.SmplTbl[i].Sample = DACFiles[i].resid - IDR_DAC_81;
			smpscfg.DACDrv.SmplTbl[i].Rate = DACFiles[i].rate;
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MISC_INSSET), _T("MISC"));
		smpscfg.GlbInsLen = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg.GlbInsData = (UINT8 *)LockResource(LoadResource(moduleHandle, hres));

		UINT16 InsCount = smpscfg.GlbInsLen / smpscfg.InsRegCnt;
		smpscfg.GlbInsLib.InsCount = InsCount;
		smpscfg.GlbInsLib.InsPtrs = NULL;
		if (InsCount)
		{
			smpscfg.GlbInsLib.InsPtrs = new UINT8*[InsCount];
			UINT16 CurPos = 0x0000;
			for (UINT16 CurIns = 0x00; CurIns < InsCount; CurIns ++, CurPos += smpscfg.InsRegCnt)
				smpscfg.GlbInsLib.InsPtrs[CurIns] = &smpscfg.GlbInsData[CurPos];
		}
		smpscfg.GlbInsBase = 0x17D8;

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

	BOOL load_song(unsigned char id, unsigned int bgmmode)
	{
		HRSRC hres;
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
			if (newid == MusicID_LavaReef2)
				if (Current_zone_and_act == hidden_palace_zone
					|| Current_zone_and_act == hidden_palace_shrine)
					newid = MusicID_HiddenPalace;
			char set = trackSettings[GameSelection][newid];
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
		musicentry *song = &MusicFiles[newid];
		if (song->mode == TrackMode_S3)
		{
			smpscfg.VolEnvs = VolEnvs_S3;
			smpscfg.DACDrv.SmplTbl[0xB2-0x81].Sample = IDR_DAC_B2_S3 - IDR_DAC_81;
			smpscfg.DACDrv.SmplTbl[0xB3-0x81].Sample = IDR_DAC_B2_S3 - IDR_DAC_81;
		}
		else
		{
			smpscfg.VolEnvs = VolEnvs_SK;
			smpscfg.DACDrv.SmplTbl[0xB2-0x81].Sample = IDR_DAC_B2 - IDR_DAC_81;
			smpscfg.DACDrv.SmplTbl[0xB3-0x81].Sample = IDR_DAC_B2 - IDR_DAC_81;
		}

		UINT8 i;
		if (! bgmmode && fmdrum_on)
		{
			for (i = 1; i <= LengthOfArray(FMDrumList); i++)
			{
				if (FMDrumList[i - 1])
				{
					smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_FM;
					smpscfg.DrumLib.DrumData[i].DrumID = FMDrumList[i - 1] - 0x81;
				}
			}
			if (song->mode == TrackMode_S3D)
			{
				smpscfg.DrumLib.DrumData[0x9F-0x80].Type = DRMTYPE_FM;
				smpscfg.DrumLib.DrumData[0x9F-0x80].DrumID = 0x86 - 0x81;
				smpscfg.DrumLib.DrumData[0xA0-0x80].Type = DRMTYPE_DAC;
				smpscfg.DrumLib.DrumData[0xA0-0x80].DrumID = S3D_ID_BASE + (0xA0-0x9F);
			}
		}
		else if (song->mode == TrackMode_S3D)
		{
			for (i = 1; i < 0x1F; i++)
			{
				smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
				smpscfg.DrumLib.DrumData[i].DrumID = i - 1;	// set DAC sample
			}
			for (; i <= 0x20; i++)
			{
				smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
				smpscfg.DrumLib.DrumData[i].DrumID = S3D_ID_BASE + (i - 0x1F);	// S3D sounds 9F/A0 start are C5/C6 here.
			}
			for (; i < smpscfg.DrumLib.DrumCount; i++)
				smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_NONE;
		}
		else
		{
			for (i = 1; i < smpscfg.DrumLib.DrumCount; i++)
			{
				smpscfg.DrumLib.DrumData[i].Type = DRMTYPE_DAC;
				smpscfg.DrumLib.DrumData[i].DrumID = i - 1;	// set DAC sample
			}
		}

		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_MUSIC_1 + newid), _T("MUSIC"));
		smpscfg.SeqBase = song->base;
		smpscfg.SeqLength = (UINT16)SizeofResource(moduleHandle, hres);
		smpscfg.SeqData = (UINT8*)LockResource(LoadResource(moduleHandle, hres));
		PreparseSMPSFile(&smpscfg);
		return TRUE;
	}

	BOOL play_song()
	{
		if (trackMIDI)
			return MIDIFallbackClass->play_song();
		ThreadPauseConfrm = false;
		PauseThread = true;
		while(! ThreadPauseConfrm)
			Sleep(1);
		PauseStream(false);

		SmplsPerFrame = SampleRate / FrameDivider;
		PlayMusic(&smpscfg);
		PauseThread = false;
		return TRUE;
	}

	BOOL stop_song()
	{
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

	__declspec(dllexport) BOOL PlaySong(unsigned char song)
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

	void NotifySongStopped()
	{
		if (EnableSKCHacks)
			PostMessageA(gameWindow, 0x464u, 0, 0);
		else if (SongStoppedCallback)
			SongStoppedCallback();
	}
}