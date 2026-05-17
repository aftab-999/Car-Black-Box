#include<xc.h>
#include"header.h"


void init_uart(unsigned long int baud){
    TRISC6 = 1 ;  // TX pin must be 1 for USART to take control (datasheet req)
    TRISC7 = 1 ;  // RX pin must be 1
    
    TX9 = 0 ; // 8 bit transmission enable 
    TXEN = 1 ; // Transmit enable bit
    SYNC = 0 ; // Asynchronous mode
    BRGH = 1 ; // High speed Asynchronous mode
    SPEN = 1 ; // Making RC6 & RC7 pins as serial ports

    SPBRG = (FOSC / (16 * baud)) - 1 ; // baudrate = (FOSC / (16 * Baud Rate))-1 
}