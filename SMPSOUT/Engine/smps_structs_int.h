#ifndef __SMPS_STRUCTS_INT_H__
#define __SMPS_STRUCTS_INT_H__

#include "stdtype.h"
#include "smps_structs.h"

// --- Internal SMPS structures ---

// SMPS Variables
// --------------
typedef struct _pan_animation
{
	UINT8 Type;		// 11 - Animation Type
	UINT8 Anim;		// 12 - Animation ID (data array)
	UINT8 AniIdx;	// 13 - Data Index
	UINT8 AniLen;	// 14 - Data Length
	UINT8 ToutInit;	// 15 - Initial Timeout value (reset value)
	UINT8 Timeout;	// 16 - Timeout
} PAN_ANIM;
typedef struct _custom_modulation
{
	UINT16 DataPtr;		// 20/21 - Modulation Settings Pointer
	UINT16 Freq;		// 22/23 - Modulation Frequency (added to Note Frequency)
	UINT8 Delay;		// 24 - initial Delay
	UINT8 Rate;			// 25 - Refresh Rate (frequency is sent every x frames)
	INT8 Delta;			// 26 - Modulation Delta
	UINT8 RemSteps;		// 27 - Remaining Steps before Delta is negated
} CUSTOM_MOD;
typedef struct _lfo_modulation
{
	UINT8 CurFMS;		// 11 - current Pan/AMS/FMS value
	UINT8 Delay;		// 12 - delay before Modulation starts
	UINT8 DelayInit;	// 13 - reset value for Delay
	UINT8 Timeout;		// 14 - remaning frames until FMS increment
	UINT8 ToutInit;		// 15 - delay between FMS increments (reset value for Timeout)
	UINT8 MaxFMS;		// 16 - destination FMS value
} LFO_MOD;

#define TRK_STACK_SIZE		8
#define PBKFLG_SPCMODE		0x01	// Bit 0 - Special Mode (Special FM 3 or Noise)
#define PBKFLG_HOLD			0x02	// Bit 1 - Hold Note
#define PBKFLG_OVERRIDDEN	0x04	// Bit 2 - overridden by SFX
#define PBKFLG_RAWFREQ		0x08	// Bit 3 - raw frequency mode
#define PBKFLG_ATREST		0x10	// Bit 4 - at rest
#define PBKFLG_PITCHSLIDE	0x20	// Bit 5 - Pitch Slide
#define PBKFLG_LOCKFREQ		0x40	// Bit 6 - lock frequence
#define PBKFLG_ACTIVE		0x80	// Bit 7 - track is active
typedef struct _track_ram
{
	SMPS_CFG* SmpsCfg;
	
	UINT8 PlaybkFlags;	// 00 - Playback Flags
	UINT8 ChannelMask;	// 01 - Channel Bits (various chip-dependent bits)
	UINT8 TickMult;		// 02 - Tick Multiplier
	UINT16 Pos;			// 03/04 - Track Pointer
	INT8 Transpose;		// 05 - move all notes up/down by this value
	UINT8 Volume;		// 06 - Track Volume
	UINT8 ModEnv;		// 07 - Modulation Envelope
	UINT8 Instrument;	// 08 - FM/PSG Instrument
	UINT8 StackPtr;		// 09 - GoSub Stack Pointer
	UINT8 PanAFMS;		// 0A - Pan/AMS/FMS bits (YM2612 Register B4)
	UINT8 Timeout;		// 0B - current Note Timeout
	UINT8 NoteLen;		// 0C - last Note Length (Timeout gets set to NoteLen when playing a note)
	union
	{
		UINT16 Frequency;	// 0D/0E - current Frequency value
		struct
		{
			UINT8 Snd;		// 0D - DAC sound
			UINT8 Unused;	// 0E - unused
		} DAC;
	};
	UINT8 FMInsSong;	// 0F - Song ID of the song that contains the FM instrument data
	INT8 Detune;		// 10 - Frequency Detune or Pitch Slide Speed
	PAN_ANIM PanAni;	// 11-16 - Pan Animation
	LFO_MOD LFOMod;		// 11-16 - LFO Modulation (Ghostbusters only)
	UINT8 VolEnvIdx;	// 17 - Volume Envelope Index
	UINT8 VolEnvCache;	// [not in driver]
	union
	{
		struct _ssg_eg_data
		{
			UINT8 Type;		// 18 - SSG-EG Enable (80 = enable)
			UINT16 DataPtr;	// 19/1A - SSG-EG Data Pointer
		} SSGEG;
		struct _fm_vol_env
		{
			UINT8 VolEnv;	// 18 - Volume Envelope
			UINT8 OpMask;	// 19 - Operator Mask (envelope will affect operators with set bit)
		} FMVolEnv;
	};
	UINT8 NoiseMode;	// 1A - PSG Noise Mode
	UINT8 FMAlgo;		// 1B - FM Algorithm (unused on SMPS Z80)
	const UINT8* VolOpPtr;	// 1C/1D - Volume Operator Data Pointer (a real pointer this time)
	UINT8 NStopTout;	// 1E - Note Stop Timeout
	UINT8 NStopInit;	// 1F - Note Stop Initial value
	UINT8 NStopRevMode;	// 2C - Reversed Note Stop Mode
	CUSTOM_MOD CstMod;	// 20-27 - Custom Modulation
	UINT8 ModEnvMult;	// 22 - Modulation Envelope Multiplier
	UINT8 ModEnvIdx;	// 25 - Modulation Envelope Index
	INT16 ModEnvCache;	// [not in driver]
	
	UINT8 SpcDacMode;
	INT8 PS4_DacMode;	// DAC On/Off, Volume Control On/Off
	UINT8 PS4_AltTrkMode;
	UINT8 GA3_DacMode;	// 2-note DAC mode on/off
	
	UINT8 LoopStack[TRK_STACK_SIZE];	// 28-2F - Loop Data and GoSub Stack
	UINT16 LoopOfs;		// [not in driver] Loop Offset (for loop detection)
} TRK_RAM;


enum MUSIC_TRACKS
{
	TRACK_MUS_DRUM,
	TRACK_MUS_DAC2,	// for Ristar
	TRACK_MUS_FM1,
	TRACK_MUS_FM2,
	TRACK_MUS_FM3,
	TRACK_MUS_FM4,
	TRACK_MUS_FM5,
	TRACK_MUS_FM6,
	TRACK_MUS_PSG1,
	TRACK_MUS_PSG2,
	TRACK_MUS_PSG3,
	TRACK_MUS_PWM1,
	TRACK_MUS_PWM2,
	TRACK_MUS_PWM3,
	TRACK_MUS_PWM4,
	MUS_TRKCNT
};
enum SFX_TRACKS
{
	TRACK_SFX_FM3,
	TRACK_SFX_FM4,
	TRACK_SFX_FM5,
	TRACK_SFX_FM6,	// SMPS Z80 only
	TRACK_SFX_PSG1,
	TRACK_SFX_PSG2,
	TRACK_SFX_PSG3,
	SFX_TRKCNT
};
enum SPCSFX_TRACKS
{
	TRACK_SPCSFX_FM4,
	TRACK_SPCSFX_PSG3,
	SPCSFX_TRKCNT
};


typedef struct _fade_info
{
	UINT8 Steps;	// 1C0D - remaining steps
	UINT8 DlyInit;	// 1C0E - initial Timeout value
	UINT8 DlyCntr;	// 1C0F - Timeout Counter
} FADE_INF;

#define TRKMODE_MUSIC	0x00
#define TRKMODE_SFX		0x01
#define TRKMODE_SPCSFX	0x80
typedef struct _sound_ram
{
	SMPS_CFG* MusCfg;
	SMPS_CFG* SFXCfg[SFX_TRKCNT + SPCSFX_TRKCNT];
	SMPS_CFG DrumCfg[0x02];
	
	// SMPS Z80 Type 1:
	// 1C00/1C01 - Data Bank
	// 1C02/1C03 - Pointer List Offset
	UINT16 TimerAVal;	// 1C04/1C05 - YM2612 Timer A Value (Music Timing)
	UINT8 TimerBVal;	// 1C06 - YM2612 Timer B Value (for SFX Timing)
	// 1C07 - Timing Mode:
	//		00 - update all on Vertical Interrupt (NTSC: 60 Hz/PAL: 50 Hz)
	//		20 - update all when YM2612 Timer A expires [not in actual driver]
	//		40 - update all when YM2612 Timer B expires (Note: often used with Timer B = CBh)
	//		80 - update music when Timer A expires, update SFX when Timer B expires
	UINT8 TimingMode;
	UINT8 LockTimingMode;
	// 1C08 - some Status flag
	//		00 - while executing StopAllSound
	//		20 - while executing StopSpecialSFX
	//		40 - after executing StopDrumPSG (happens when starting FadeOut)
	//		80 - else
	UINT8 DacState;		// [not in sound driver] keeps track of the current DAC state
	UINT8 DacChVol[2];	// 1C07/1C08 - Chou Yakyuu Miracle Nine DAC channel volume
	
	// SMPS Z80 Type 2:
	// 1C00/1C01 - ??? (unused)
	// 1C02/1C03 - Pointer List Offset
	// 1C04/1C05 - DAC Bank
	// 1C06/1C07 - Music/SFX Data Bank
	// 1C08 - some Status flag
	//		81 - Processing Sound Queue
	//		82 - during Bank Switching process
	
	// 1C09 - PlaySound ID (00 = StopAllSound, 80 = do nothing)
	// 1C0A-0C - Sound Queue (sound IDs that get checked against Sound Priority and moved to 1C09)
	FADE_INF FadeOut;
	
	UINT8 PauseMode;		// 1C10
	// 1C11 - Music is paused via in-sequence commands
	UINT8 SpcFM3Mode;		// 1C12 - Special FM3 Mode Bits (YM2612 Register 027)
	UINT8 TempoCntr;		// 1C13 - Tempo Counter/Tempo Timeout
	UINT8 TempoInit;		// 1C14 - Initial Tempo Value
	// 1C15 - unknown (set to 00 when SFX is started, set to 1F when a track ends)
	UINT8 CommData;			// 1C16 - Communication Data Byte
	// 1C17 - unknown (set to a random value (Z80 register R) when executing DoSoundQueue
	UINT8 CurSFXPrio;		// 1C18 - current SFX Priority
	UINT8 TrkMode;			// 1C19 - Music/SFX Mode
	
	UINT16 FM3Freqs_SpcSFX[4];	// 1C1A - Special FM3 Frequencies: Special SFX
	UINT16 FM3Freqs_SFX[4];		// 1C22 - Special FM3 Frequencies: SFX
	UINT16 FM3Freqs_Mus[4];		// 1C2A - Special FM3 Frequencies: Music
	
	UINT8 CondJmpVal;
	
	// 1C32 - GetSFXChnPtrs saves the current Channel ID to this byte
	// 1C33 - PlayMusic variable: Track Header offset
	// 1C35 - PlayMusic variable: pointer to FM/PSG Initialisation Bytes
	// 1C37 - Music Instrument Pointer (see MusCfg.InsLib)
	// 1C39 - PlaySFX varaible: Instrument Pointer (never read back)
	// 1C3B - PlaySFX variable: Tick Multiplier
	// 1C3C - queued DAC sound
	// 1C3D - DAC sound Bank Number
	// 1C3E-3F - unused
	
	TRK_RAM MusicTrks[MUS_TRKCNT];		// 1C40 - Music Tracks
	TRK_RAM SFXTrks[SFX_TRKCNT];		// 1DF0 - SFX Tracks
	TRK_RAM SpcSFXTrks[SPCSFX_TRKCNT];	// 1F40 - Special SFX Tracks
} SND_RAM;


#endif // __SMPS_STRUCTS_INT_H__