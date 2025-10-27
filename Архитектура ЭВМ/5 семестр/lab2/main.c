#include <LPC24xx.H>

#define FIRST_POGR   0x00000001  // P0.0 
#define SECOND_POGR  0x00000002  // P0.1 
#define MIXER        0x00000004  // P0.2 
#define BUTTON_PIN   0x00000008  // P0.8 

void Timer0_Init(void);
void delay_ms(unsigned int milliseconds);
void delay_500ms(void);
void delay_10s(void);
unsigned int is_button_pressed(void);
void mixing_and_pogr(void);
void second_pogr_start(void);
void baking_mode(void);

void SystemInit(void) {
    PLLCON &= 0xFFFFFFFE;
    while (PLLSTAT & 0x02000000);
    
    PLLCFG = 0x00000005;  
    
    PLLFEED = 0x000000AA;
    PLLFEED = 0x00000055;
    PLLCON |= 0x00000001;
    PLLFEED = 0x000000AA;
    PLLFEED = 0x00000055;
    
    while (!(PLLSTAT & 0x04000000));
    
    PLLCON |= 0x00000002;
    PLLFEED = 0x000000AA;
    PLLFEED = 0x00000055;
    
    CCLKCFG = 0x00000003;  
	
	CLKSRCSEL = 0x00000001;
    
    PCLKSEL0 = 0x00000000;
    PCLKSEL1 = 0x00000000;
}

void Timer0_Init(void) {
    PCONP |= (1 << 1);
    
    T0TCR = 0x02;
    T0PR = 3;
    T0MCR = 0x00000006;
    
    T0TC = 0;
}

void delay_ms(unsigned int milliseconds) {
    T0MR0 = 726 * milliseconds;
    
    T0TC = 0;
    
    T0TCR = 0x01;
    
    while ((T0TCR & 0x01) != 0);
}

void delay_500ms(void) {
    delay_ms(500);
}

void delay_10s(void) {
    delay_ms(10000);
}

unsigned int is_button_pressed(void) {
    return !(IOPIN0 & BUTTON_PIN);
}

void mixing_and_pogr(void) {
    IOSET0 = MIXER | FIRST_POGR;
    
    delay_500ms();
    
    IOCLR0 = MIXER | FIRST_POGR;
}

void second_pogr_start(void) {
    IOSET0 = SECOND_POGR;
    
    delay_500ms();
    
    IOCLR0 = SECOND_POGR;
}

void baking_mode(void) {
    IOSET0 = SECOND_POGR;
    
    delay_10s();
    
    IOCLR0 = SECOND_POGR;
    IOSET0 = BUTTON_PIN;
}

int main(void) {
    PINSEL0 = 0x00000000; 
    PINSEL1 = 0x00000000;

	SystemInit();
    
    IODIR0 = FIRST_POGR | SECOND_POGR | MIXER;
    
    IOCLR0 = FIRST_POGR | SECOND_POGR | MIXER;
    
    Timer0_Init();
    
    while(1) {
        mixing_and_pogr();
        
        delay_500ms();
        
        if (is_button_pressed()) {
            baking_mode();
            
            while(is_button_pressed()) {
                delay_ms(50);
            }
        }
        else {
            second_pogr_start();
        }
    }
}