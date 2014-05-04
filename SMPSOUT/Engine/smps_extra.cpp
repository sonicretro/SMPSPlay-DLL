#include <stddef.h>	// for NULL
#include <malloc.h>
#include <memory.h>
#include "stdtype.h"
#include "smps_structs_int.h"
#include "dac.h"

//void StartSignal(void);
//void StopSignal(void);
//void LoopStartSignal(void);
//void LoopEndSignal(void);

//void Extra_StopCheck(void);
//void Extra_LoopStartCheck(TRK_RAM* Trk);
//void Extra_LoopEndCheck(TRK_RAM* Trk);

extern UINT32 PlayingTimer;
INT32 LoopCntr;
extern INT32 StoppedTimer;

extern UINT8 VGM_DataBlkCompress;

extern SND_RAM SmpsRAM;
static struct loop_state
{
	UINT8 Activated;
	UINT16 TrkMask;
	UINT16 TrkPos[MUS_TRKCNT];
} LoopState;


void StartSignal(void)
{
	UINT8 CurTrk;
	
	PlayingTimer = -1;
	LoopCntr = 0;
	StoppedTimer = -1;
	
	LoopState.Activated = 0xFF;
	for (CurTrk = 0; CurTrk < MUS_TRKCNT; CurTrk ++)
	{
		if (SmpsRAM.MusicTrks[CurTrk].LoopOfs)
		{
			LoopState.Activated = 0x00;
			break;
		}
	}
	LoopState.TrkMask = 0x0000;
	
	return;
}

void StopSignal(void)
{
	LoopCntr = -1;
	StoppedTimer = 0;
	
	return;
}

void LoopStartSignal(void)
{
	LoopCntr = 1;
	
	return;
}

void LoopEndSignal(void)
{
	LoopCntr ++;
	
	return;
}

extern void NotifySongStopped();
void Extra_StopCheck(void)
{
	UINT8 CurTrk;
	UINT16 TrkMask;
	
	if (SmpsRAM.TrkMode != TRKMODE_MUSIC)
		return;
	
	TrkMask = 0x0000;
	for (CurTrk = 0; CurTrk < MUS_TRKCNT; CurTrk ++)
	{
		if (SmpsRAM.MusicTrks[CurTrk].PlaybkFlags & PBKFLG_ACTIVE)
			TrkMask |= (1 << CurTrk);
	}
	
	if (! TrkMask)
	{
		StopSignal();
		NotifySongStopped();
	}
	
	return;
}

void Extra_LoopStartCheck(TRK_RAM* Trk)
{
	UINT8 TrkID;
	UINT8 CurTrk;
	TRK_RAM* TempTrk;
	
	if (SmpsRAM.TrkMode != TRKMODE_MUSIC)
		return;
	if (LoopState.Activated)
		return;
	if (! Trk->LoopOfs)
		return;
	
	TrkID = (UINT8)(Trk - SmpsRAM.MusicTrks);
	if (Trk->Pos != Trk->LoopOfs)
		return;
	
	LoopState.TrkMask |= (1 << TrkID);
	for (CurTrk = 0; CurTrk < MUS_TRKCNT; CurTrk ++)
	{
		TempTrk = &SmpsRAM.MusicTrks[CurTrk];
		if (! (TempTrk->PlaybkFlags & PBKFLG_ACTIVE) || ! TempTrk->LoopOfs)
			continue;
		
		if (! (LoopState.TrkMask & (1 << CurTrk)))
			return;	// One channel is still outside of a loop
		
		LoopState.TrkPos[CurTrk] = TempTrk->Pos;
	}
	
	LoopState.Activated = 0x01;
	LoopState.TrkMask = TrkID;
	LoopStartSignal();
	
	return;
}

void Extra_LoopEndCheck(TRK_RAM* Trk)
{
	UINT8 TrkID;
	UINT8 CurTrk;
	TRK_RAM* TempTrk;
	
	if (SmpsRAM.TrkMode != TRKMODE_MUSIC)
		return;
	if (LoopState.Activated < 0x01 || LoopState.Activated == 0xFF)
		return;
	
	TrkID = (UINT8)(Trk - SmpsRAM.MusicTrks);
	if (TrkID != LoopState.TrkMask)
		return;
	
	for (CurTrk = 0; CurTrk < MUS_TRKCNT; CurTrk ++)
	{
		TempTrk = &SmpsRAM.MusicTrks[CurTrk];
		if (! (TempTrk->PlaybkFlags & PBKFLG_ACTIVE) || ! TempTrk->LoopOfs)
			continue;
		
		if (TempTrk->Pos != LoopState.TrkPos[CurTrk])
			return;
	}
	
	LoopState.Activated = 0x02;
	LoopEndSignal();
	
	return;
}