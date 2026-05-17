#include<xc.h>
#include "header.h"

unsigned short int read_adc(unsigned char channel){
    unsigned short int reg_value ;
    // 01 - ADRESH  10100100 - ADRESL
    ADCON0 = ADCON0 & 0xC7 | (channel << 3); // clear last 3 bits and set the channel number
    __delay_us(20); //acquisition time (internal capacitor gets charge till it gets equal to vin ).
    GO = 1 ; // start adc conversion 

    while(nDONE);  // wait  for adc conversion

    reg_value = (ADRESH<<8)|ADRESL;  // read adc value in 10 bit register
    return reg_value ; 
}
void init_adc(){
    ADCON1 = 0x8E; // ADFM=1 (Right justified), PCFG=1110 (AN0 is analog, others digital)
    
    ADCS2 = 0 ;
    ADCS1 = 1 ; // FOSC / 32 (clock conversion)
    ADCS0 = 0 ;
    
    ADON = 1 ;  // A/D converter module is powered up 

    TRISA0 = 1; // making TRISA0 as input becoz its connected to potentiometer
}
