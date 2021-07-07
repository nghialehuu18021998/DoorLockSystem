#ifndef __KEYPAD__
#define __KEYPAD__
#include <NUC100Series.h>
void OpenKeyPad(void);
void CloseKeyPad(void);
uint8_t ScanKey(void);
void Wait_for_key(void);
#endif
