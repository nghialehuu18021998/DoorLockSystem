#ifndef __MISC__
#define __MISC__
#include "NUC100Series.h"
void Enable_GB15(void);
void SysInit(void);
void TMR0_EnableInt(void);
void Enable_IRQ0_NVIC(void);
void ClearIntFlag_TIMER0(void);
void Init_Timer0(void);
void Set_Duration_Timer0(uint32_t ms);
void Start_Timer0(void);
#endif
