#include <stdio.h>
#include "NUC100Series.h"
#include "LCD.h"
#include "Keypad.h"
#include "misc.h"
#include "string.h"

#define NUMBER_OF_STAGE 6
#define NO_PASSWORD_DIGIT 6
#define MAX_STRING_SIZE 25

#define PASSWORD_LENGTH 6
#define DEBOUNCE_KEYPAD_TIME 200

#define STAGE_MAIN_MENU 0
#define STAGE_UNLOCK 1
#define STAGE_CHANGE_KEY 2
#define STAGE_UNLOCKED 3
#define STAGE_INCORRECT 4
#define STAGE_CHANGED 5
#define STAGE_RESET 6


char password[NO_PASSWORD_DIGIT] = {'1', '2', '3', '4', '5', '6'};
char secure_password_buffer[NO_PASSWORD_DIGIT];
volatile char password_buffer[NO_PASSWORD_DIGIT] = {'_', '_', '_', '_', '_', '_'};

volatile uint8_t current_stage = 0;
uint8_t is_unlocked = 0;

uint8_t typing_index = 0;
uint8_t can_type = 1;

char screen_display[NUMBER_OF_STAGE][MAX_STRING_SIZE];

void Reset_Password_Buffer(void);
void Set_New_Password(void);
void Reset_Password(void);

void Delay_s(uint32_t count) {
	uint32_t  n;
	for (n= 0; n < count; n++) {
	}
}

void Draw_screen(void) {
	int8_t i;
	for (i = 0; i < 6; i++) {
		printS_5x7(0 ,10 * i , screen_display[i]);
	}
}

void Draw_Password(void) {
	int8_t i, x;
	x = 0;
	for (i = 0; i < 6; i++) {
		printC_5x7(x, 50, password_buffer[i]);
		x = x + 12;
	}
}

void Draw_Screen(void) {
	clear_LCD();
	switch (current_stage) {
		case STAGE_MAIN_MENU:
			strcpy(screen_display[0], "EEET2481 - Group 19");
			strcpy(screen_display[1], "Door Lock System");
			strcpy(screen_display[2], "Select from the following");
			strcpy(screen_display[3], "options: ");
			strcpy(screen_display[4], "1. Unlock");
			strcpy(screen_display[5], "2. Change Key");
			break;
		case STAGE_UNLOCK:
			strcpy(screen_display[2], "");
			strcpy(screen_display[3], "Enter the key code");
			strcpy(screen_display[4], "");
			strcpy(screen_display[5], "");
			Draw_Password();
			break;
		case STAGE_CHANGE_KEY:
			if (is_unlocked) {
				strcpy(screen_display[2], "Enter a new 6 digit");
				strcpy(screen_display[3], "key code");
			} else {
				strcpy(screen_display[2], "Enter the key code");
				strcpy(screen_display[3], "");
			}
			
			strcpy(screen_display[4], "");
			strcpy(screen_display[5], "");
			Draw_Password();
			break;
		case STAGE_UNLOCKED:
			strcpy(screen_display[3], "");
			strcpy(screen_display[4], "Key code correct");
			strcpy(screen_display[5], "Door Unlocked");
			break;
		case STAGE_INCORRECT:
			strcpy(screen_display[3], "Key Code Incorrect");
			strcpy(screen_display[4], "System restarts in");
			strcpy(screen_display[5], "1 second");
			break;
		case STAGE_CHANGED:
			strcpy(screen_display[3], "");
			strcpy(screen_display[4], "Key code has been changed");
			strcpy(screen_display[5], "");
			break;
		case STAGE_RESET:
			strcpy(screen_display[3], "");
			strcpy(screen_display[4], "Setting has been resetted");
			strcpy(screen_display[5], "");
			break;
		default: break;	
	}
	Draw_screen();
}


int Is_Valid_Password(void) {
	int i = 0;
	for (i = 0; i < 6; i++) {
		if (password[i] != secure_password_buffer[i]) {
			return 0;
		}
	}
	return 1;
}

char to_char(int8_t i) {
	return '0' + i;
}

void Handle_Keyin(int8_t keyin) {
	char c;
	if (typing_index < PASSWORD_LENGTH) {
		if (keyin > 0 && can_type) {
			c = to_char(keyin);
			secure_password_buffer[typing_index] = c;
			password_buffer[typing_index++] = c;
			can_type = 0;
			Wait_for_key();
			Draw_Screen();
			Set_Duration_Timer0(DEBOUNCE_KEYPAD_TIME);
			Start_Timer0();
		}
	}
}

void Reset_Buffer(void) {
	typing_index = 0;
	can_type = 1;
	Reset_Password_Buffer();
}

int main(void)
{
	int8_t keyin=0;
	
	SysInit();	
	
	//Enable button 
	Enable_GB15();
	
	//Enable timer 0
	Init_Timer0();
	
	//Init LCD
    init_LCD();
    clear_LCD();

	//Init Keypad
	OpenKeyPad();
	
	Draw_Screen();
	while(1) {
		switch (current_stage) {
			case STAGE_MAIN_MENU:
				keyin = ScanKey();
				if (keyin) {
					Wait_for_key();
					if (keyin == 1) {
						current_stage = STAGE_UNLOCK;
					} else if (keyin == 2) {
						current_stage = STAGE_CHANGE_KEY;
					}
					Draw_Screen();
				}
				break;
			case STAGE_UNLOCK:
				keyin = ScanKey();
				Handle_Keyin(keyin);
				if (typing_index == PASSWORD_LENGTH) {
					Delay_s(1200000);
					if (Is_Valid_Password()) {
						current_stage = STAGE_UNLOCKED;
						Set_Duration_Timer0(3000);
					} else {
						current_stage = STAGE_INCORRECT;
						Set_Duration_Timer0(1600);
					}
					
					Draw_Screen();
					Start_Timer0();
					Reset_Buffer();
				}
				break;
			case STAGE_CHANGE_KEY:
				keyin = ScanKey();
				Handle_Keyin(keyin);
				if (typing_index == PASSWORD_LENGTH) {
					Delay_s(1200000);
					if (!is_unlocked) {
						if (!Is_Valid_Password()) {
							current_stage = STAGE_INCORRECT;
							Set_Duration_Timer0(2000);
						} else {
							is_unlocked = 1;
						}						
					}
					else {
						Set_New_Password();
						current_stage = STAGE_CHANGED;
						Set_Duration_Timer0(2000);
						is_unlocked = 0;
					}
					Draw_Screen();
					Start_Timer0();
					Reset_Buffer();
				}
				break;
			case STAGE_UNLOCKED: break;
			case STAGE_INCORRECT: break;
			case STAGE_CHANGED: break;
			default: break;	
		}
		//Draw_Screen();
	};
}

void EINT1_IRQHandler(void){
	current_stage = STAGE_RESET;
	Draw_Screen();
	Reset_Password();
	Reset_Password_Buffer();
	Set_Duration_Timer0(2000);
	Start_Timer0();
	//clear interrupt flag
	PB->ISRC |= (1ul << 15);
}

void TMR0_IRQHandler(void){
	if (current_stage == STAGE_INCORRECT || current_stage == STAGE_UNLOCKED) {
		current_stage = STAGE_MAIN_MENU;
	} else if (current_stage == STAGE_UNLOCK || current_stage == STAGE_CHANGE_KEY) {
		if (typing_index) password_buffer[typing_index - 1] = '*';
		can_type = 1;
	} else if (current_stage == STAGE_INCORRECT) {
		current_stage = STAGE_MAIN_MENU;
	} else if (current_stage == STAGE_CHANGED) {
		current_stage = STAGE_MAIN_MENU;
	} else if (current_stage == STAGE_RESET) {
		current_stage = STAGE_MAIN_MENU;
	}
	Draw_Screen();
	ClearIntFlag_TIMER0();
}

void Reset_Password_Buffer(void) {
	uint8_t i = 0;
	for (i = 0; i< PASSWORD_LENGTH; i++) {
		password_buffer[i] = '_';
	}
}

void Set_New_Password(void) {
	uint8_t i = 0;
	for (i = 0; i< PASSWORD_LENGTH; i++) {
		password[i] = secure_password_buffer[i];
	}
}

void Reset_Password(void) {
	//Set password back to 123456
	uint8_t i = 0;
	for (i = 0; i < PASSWORD_LENGTH; i++) {
		password[i] = '0' + i + 1;
	}
}
