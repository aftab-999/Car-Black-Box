#include<xc.h>
#include"header.h"

void init_timer0(){
    TMR0IE = 0 ;
    TMR0 = 0 ;
    TMR0IF = 0 ;
    
    // nRBPU=0 (pull-ups ON), T0CS=0 (internal), PSA=0 (prescaler to TMR0), PS=111 (1:256)
    OPTION_REG = 0x07 ;
}