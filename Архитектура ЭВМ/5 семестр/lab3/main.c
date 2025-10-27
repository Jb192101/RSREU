#include <LPC24xx.h>
#include <string.h>

#define Fpclk_UART0 12000000UL
#define BAUD        19200UL
#define DLL_VALUE   (Fpclk_UART0 / (16UL * BAUD))  

volatile unsigned int tick_count = 0;  
volatile char current = 0;

void UART0_Init(void);
void UART0_Int(void) __irq;
void FIQ_Addr(void) __irq;     
void UART0_SendChar(char c);
void UART0_SendString(const char *s);
char UART0_RecvChar(void);
void Timer0_Init(void);
void delay_0_1s(void);
int is_all_zero(const char *s, int len);
void send_most_frequent_not_in_second(const char *s1, int len1, const char *s2, int len2);

void UART0_Init(void) {
    PINSEL0 &= ~((3<<4) | (3<<6));
    PINSEL0 |=  (1<<4) | (1<<6);

    U0LCR = 0x83;                     
    U0DLL = (unsigned char)DLL_VALUE;
    U0DLM = 0;
    U0LCR = 0x03;                     
    U0FCR = 0x81;                     
    U0IER = 0x01;                     

    VICVectAddr6 = (unsigned)UART0_Int;
    VICVectCntl6 = 0x20 | 6;
    VICIntEnable |= (1 << 6);
    VICIntSelect &= ~(1 << 6);        

    PCONP |= (1 << 3);               
}

void UART0_SendChar(char c) {
    while (!(U0LSR & 0x20));
    U0THR = (unsigned char)c;
}

void UART0_SendString(const char *s) {
    while (*s) UART0_SendChar(*s++);
}

char UART0_RecvChar(void) {
    while (!(U0LSR & 0x01));
    return (char)U0RBR;
}

void Timer0_Init(void) {
    PCONP |= (1 << 1);                
    T0TCR = 0x02;                    
    T0PR  = 12000 - 1;                
    T0MR0 = 100;                      
    T0MCR = 0x03;                     

    VICIntEnable |= (1 << 4);
    VICIntSelect |= (1 << 4);         
    T0TCR = 0x01;                    
}

void FIQ_Addr(void) __irq {
    tick_count++;                    
    T0IR = 1 << 0;                    
    VICVectAddr = 0;                 
}

void delay_0_1s(void) {
    volatile unsigned int dummy;
    T0TCR = 0x02;                      
    T0PR  = 24000 - 1;                
    T0MR0 = 100;                       
    T0MCR = 0x04;                      
    T0TCR = 0x01;                   
    while (T0TC < 100) { dummy = T0TC; }
    T0TCR = 0x00; 
}

void UART0_Int(void) __irq {
    unsigned int iir;
    while (!((iir = U0IIR) & 0x1)) {
        if ((iir & 0xE) == 0x4 || (iir & 0xE) == 0xC) {
            current = U0RBR;         
        }
    }
    VICVectAddr = 0;
}

int is_all_zero(const char *s, int len) {
    int i;
    for (i = 0; i < len; i++)
        if (s[i] != 0) return 0;
    return 1;
}

void send_most_frequent_not_in_second(const char *s1, int len1, const char *s2, int len2) {
    int freq[256];
    int i, j, maxf = 0, printed = 0, found;

    for (i = 0; i < 256; i++) freq[i] = 0;
    for (i = 0; i < len1; i++) freq[(unsigned char)s1[i]]++;
    for (i = 0; i < 256; i++) if (freq[i] > maxf) maxf = freq[i];

    if (maxf == 0) {
        UART0_SendString("No characters in first string.\r\n");
        return;
    }

    for (i = 0; i < 256; i++) {
        if (freq[i] != maxf) continue;
        found = 0;
        for (j = 0; j < len2; j++) {
            if ((unsigned char)s2[j] == (unsigned char)i) {
                found = 1;
                break;
            }
        }
        if (!found) {
            UART0_SendChar((char)i);
            printed = 1;
        }
    }

    if (!printed)
        UART0_SendString("(no freq chars from str1 absent in str2)\r\n");
    else
        UART0_SendString("\r\n");
}

int main(void) {
    char str1[11];
    char str2[11];
    int i;
    int k;
    char c;

    for (i = 0; i < 11; i++) {
        str1[i] = 0;
        str2[i] = 0;
    }

    UART0_Init();
    Timer0_Init();

    for (k = 0; k < 3; k++) {
        UART0_SendString("Hello\r\n");
        delay_0_1s();
    }

    UART0_SendString("Enter first string (10 bytes):\r\n");
    for (i = 0; i < 10; i++) {
        c = UART0_RecvChar();
        str1[i] = c;
    }
    UART0_SendString("\r\nString1 received\r\n");

    UART0_SendString("Enter second string (10 bytes):\r\n");
    for (i = 0; i < 10; i++) {
        c = UART0_RecvChar();
        str2[i] = c;
    }
    UART0_SendString("\r\nString2 received\r\n");

    UART0_SendString("\r\nResult: ");
    if (is_all_zero(str1, 10) || is_all_zero(str2, 10)) {
        UART0_SendString("Strings not entered.\r\n");
    } else {
        send_most_frequent_not_in_second(str1, 10, str2, 10);
    }
    UART0_SendString("Result sent\r\n");

    while (1) {}
    return 0;
}
