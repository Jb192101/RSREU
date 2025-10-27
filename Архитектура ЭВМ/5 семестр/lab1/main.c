#include <LPC24xx.H>

#define FIRST_POGR 0x00000001
#define SECOND_POGR 0x00000002
#define MIXER 0x00000004

#define BUTTON_PIN 0x00000100

void delay(void) {
  unsigned int i;
  for (i=0;i<0xfffff;i++){}
}

unsigned int is_button_pressed(void) {
  return !(IOPIN0 & BUTTON_PIN);
}

// mixing and progrev
void mixing_and_pogr(void) {
  IOSET0 = MIXER;
  
  delay();
  delay();
  
  IOCLR0 = MIXER;
  
  delay();
  delay();
}

void second_pogr_start(void) {
  IOSET0 = SECOND_POGR;
  
  delay();
  delay();
  
  IOCLR0 = SECOND_POGR;
	
	delay();
  delay();
}

void baking_mode(void) {
  IOSET0 = FIRST_POGR;
  
  delay();
  delay();
  delay();
  delay();
  
  IOCLR0 = FIRST_POGR;
}

int main (void) {
  PINSEL0 = 0x00000000; 
  PINSEL1 = 0x00000000;
  IODIR0 = 0x00000007;
	
  IOCLR0 = 0x00000007;
  
  while(1) {
    unsigned int n;
    
    IOSET0 = FIRST_POGR;
    IOSET0 = MIXER;
    
    for (n = 0x00000001; n <= 0x0000080; n <<= 1) {
			mixing_and_pogr();
      
      if (is_button_pressed()) {
        baking_mode();
        while(!is_button_pressed()) {
          delay();
        }
      } else {
        second_pogr_start();
      }
    }
  }
}