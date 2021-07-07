#include "NUC100Series.h"
#include "misc.h"
#define PRIORITY_VAL 1
#define TCMPR_VAL_ONE_SECOND 3000000

void SysInit(void) {
	uint32_t clkMask = 0x10;
	int32_t timeOutCount = 1200000;
	
	SYS_UnlockReg(); 

	// Enable HXT external 12MHz crystal
	CLK->PWRCON |= (1ul << 0);

	//Wait till clock rdy
    while((CLK->CLKSTATUS & clkMask) != clkMask)
    {
        if(timeOutCount-- <= 0)
            break;
    }
	
	//Enable Module Clock for SPI3 module
	CLK->APBCLK = 0x8001;
	
	//Enable TMR0
	CLK->APBCLK |= (1 << 2);
	
	
	SYS->GPD_MFP |= 0xF00;
	
	PD->PMD &= ~(0x03UL << 28);
	PD->PMD |= (0x02UL << 28);			// Set GPD14 to open-drain output
	PD->DOUT &= ~(0x01UL << 14);		// LCD Backlight on
	
	SYS_LockReg(); 
}

void Enable_GB15(void) {
	//GPIO Interrupt configuration. GPIO-B15 is the interrupt source
	PB->PMD &= (~(0x03ul << 30));
	PB->IMD &= (~(1ul << 15));
	PB->IEN |= (1ul << 15);
	
	//NVIC interrupt configuration for GPIO-B15 interrupt source
	NVIC->ISER[0] |= 1ul<<3;
	NVIC->IP[0] &= (~(3ul<<30));
}

void TMR0_EnableInt(void) {
	TIMER0->TCSR |= (1ul << 29);
}

void Enable_IRQ0_NVIC(void)
{
	NVIC->ISER[0] = (1 << (8 & 0x1F));
}

void ClearIntFlag_TIMER0(void) {
	TIMER0->TISR |= 1ul;
}

void Init_Timer0(void){
	//Prescale 4
	TIMER0->TCSR &= ~(0xFF);
	TIMER0->TCSR |= 3;
	
	//Periodic mode
	TIMER0->TCSR &= ~(0x3 << 27);
	TIMER0->TCSR |= (00ul <<27); //One shot mode 00
	
	//Timer0 enable
    TMR0_EnableInt();
	
	//Enable Interrupt
    Enable_IRQ0_NVIC();
	
	//Set priority to 1
	NVIC->IP[2] &= ~(PRIORITY_VAL << 6);
	NVIC->IP[2] |= (PRIORITY_VAL << 6);
}
void Set_Duration_Timer0(uint32_t ms) {
	//Compare register value
	TIMER0->TCMPR &= ~(0xFFFFFF);
	TIMER0->TCMPR |= (TCMPR_VAL_ONE_SECOND / 1000 * ms);
}
void Start_Timer0(void) {
	//Start counting
	TIMER0->TCSR |= (1ul <<30);
}
