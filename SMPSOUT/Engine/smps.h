#ifndef __SMPS_H__
#define __SMPS_H__

#ifdef __cplusplus
extern "C"
{
#endif

// --- SMPS functions for external usage ---

#include "smps_structs.h"

void InitDriver(void);

#define UPDATEEVT_VINT	0x00
#define UPDATEEVT_TIMER	0x01
void UpdateAll(UINT8 Event);

void PlayMusic(SMPS_CFG* SmpsFileConfig);
void PlaySFX(SMPS_CFG* SmpsFileConfig, UINT8 SpecialSFX);

void FadeOutMusic(void);

void StopAllSound(void);

void SetDACState(UINT8 DacOn);

#ifdef __cplusplus
}
#endif

#endif	// __SMPS_H__
