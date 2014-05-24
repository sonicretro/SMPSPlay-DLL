#ifndef __DAC_H__
#define __DAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define COMPR_PCM	0x00
#define COMPR_DPCM	0x01

typedef struct _dac_sample
{
	UINT8* Data;
	UINT8* DPCMArr;
	UINT32 Size;
	UINT8 Compr;
	UINT8 Flags;
} DAC_SAMPLE;
typedef struct _dac_table
{
	UINT16 Sample;
	UINT8 Pan;
	UINT8 reserved;
	UINT32 Rate;
	UINT32 OverriddenRate;
} DAC_TABLE;

#define DACRM_DELAY		0x00
#define DACRM_OVERFLOW	0x01
#define DACRM_NOVERFLOW	0x02
typedef struct _dac_settings
{
	UINT32 BaseCycles;
	UINT32 LoopCycles;
	UINT32 LoopSamples;
	UINT32 RateOverflow;
	
	UINT32 BaseRate;
	UINT32 Divider;	// Note: .2 (decimal) Fixed Point, i.e. 123 means 1.23
	UINT8 RateMode;
	
	UINT8 Channels;
	UINT8 VolDiv;
} DAC_SETTINGS;

typedef struct _dac_config
{
	DAC_SETTINGS Cfg;
	
	UINT16 SmplCount;
	UINT16 SmplAlloc;
	DAC_SAMPLE* Smpls;
	
	UINT16 TblCount;
	UINT16 TblAlloc;
	DAC_TABLE* SmplTbl;
} DAC_CFG;


#define DACFLAG_LOOP	0x01
#define DACFLAG_REVERSE	0x02


void SetDACDriver(DAC_CFG* DACSet);

void DAC_Reset(void);
void DAC_ResetOverride(void);

void DAC_SetFeature(UINT8 Chn, UINT8 DacFlag, UINT8 Set);
void DAC_SetRateOverride(UINT16 SmplID, UINT32 Rate);
void DAC_SetVolume(UINT8 Chn, UINT16 Volume);

void DAC_Stop(UINT8 Chn);
void DAC_Play(UINT8 Chn, UINT16 SmplID);
void DAC_SetRate(UINT8 Chn, UINT32 Rate, UINT8 MidNote);
void DAC_SetFrequency(UINT8 Chn, UINT32 Freq, UINT8 MidNote);

void UpdateDAC(UINT32 Samples);

#ifdef __cplusplus
}
#endif

#endif	// __DAC_H__
