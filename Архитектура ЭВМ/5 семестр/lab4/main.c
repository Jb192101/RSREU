#include <LPC24xx.H>
#include <math.h>

#define ADC_CLKDIV 311
#define PI 3.14159265358979323846

volatile unsigned short adc0_val, adc1_val, modulated_val;
volatile unsigned short signal1, signal2;
volatile unsigned int counter = 0;

void ADC_Init(void) {
    PINSEL1 |= (1 << 14) | (1 << 16);
    AD0CR = (0x03 << 0) | (ADC_CLKDIV << 8) | (1 << 16) | (1 << 21);
}

void DAC_Init(void) {
    PINSEL1 |= (1 << 21);
    DACR = 0x00000000; 
}

void Timer0_Init(void) {
    T0PR = 49999;     
    T0TCR = 0x02;     
    T0TCR = 0x01;   
}

void delay_ms(unsigned int ms) {
    T0TC = 0;        
    while (T0TC < ms); 
}

unsigned short generate_sine(unsigned int angle, float frequency_multiplier) {
    double angle_rad = (angle * frequency_multiplier * 2 * PI) / 360.0;
    double sine_value = sin(angle_rad);
    return (unsigned short)((sine_value + 1.0) * 511.5);
}

unsigned short generate_cosine(unsigned int angle, float frequency_multiplier) {
    double angle_rad = (angle * frequency_multiplier * 2 * PI) / 360.0;
    double cosine_value = cos(angle_rad);
    return (unsigned short)((cosine_value + 1.0) * 511.5);
}

unsigned short readADC(int channel) {
    unsigned int data;
    do {
        data = AD0GDR;
    } while (((data >> 24) & 0x7) != channel);
    return (data >> 6) & 0x3FF;
}

void writeDAC(unsigned short value) {
    DACR = (value & 0x3FF) << 6;
}

int main(void) {
    unsigned long modulated;
    unsigned int angle = 0;
    
    ADC_Init();
    DAC_Init();
    Timer0_Init(); 
    
    while (1) {
        counter++;
        if (counter > 1023) counter = 0;
        
        angle = (angle + 2) % 360;
        
        adc0_val = readADC(0);
        adc1_val = readADC(1);
        
        if (adc0_val < 20 && adc1_val < 20) {
            signal1 = generate_sine(angle, 1.0f);
            signal2 = generate_cosine(angle, 2.0f);
        } else {
            signal1 = adc0_val;
            signal2 = adc1_val;
        }
        
        modulated = (signal1 * signal2) / 1024;
        
        if (modulated > 1023) modulated = 1023;
        
        modulated_val = (unsigned short)modulated;
        
        writeDAC(modulated_val);
        
        delay_ms(1); 
    }
}
