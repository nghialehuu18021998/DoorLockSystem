#include <stdio.h>
#include <NUC100Series.h>
#include "Keypad.h"

void OpenKeyPad(void)
{
	//Set PA0 to 5 mode to quasi bidirectional mode
	PA -> PMD &= ~(0x4095);
    PA -> PMD |= (0x4095);					
}

int extractBit(uint32_t byte, uint8_t pos) {
	//extract bit value at position pos
   return (byte >> pos) & 0x01;
}

uint8_t ScanKey(void)
{
	uint8_t i, j;
	for (i = 0; i < 3; i++) {
		PA->DOUT |= (0xFFFF);
		PA->DOUT &= ~(0x1 << (2 - i));
		for (j = 0; j < 3; j++){
			if (extractBit(PA->PIN, 3 + j) == 0) return 1 + i + j * 3;
		}
	}
	return 0;
}

void Wait_for_key(void){
	//check when user release the button
	int8_t keyin = ScanKey();
	while(keyin) {
		keyin = ScanKey();
	}
}
