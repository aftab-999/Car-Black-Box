#include <xc.h>
#include "header.h"

void __interrupt() isr(){
 static unsigned char overflow_count = 0 ;
 
 if(TMR0IF){
    TMR0IF = 0 ;
    overflow_count++ ;
    
    if(overflow_count >= 45){
        overflow_count = 0 ;
    
        if(blocked_seconds > 0){
            blocked_seconds--;
        }
        if(blocked_seconds == 0){
            blocked = 0 ;
            TMR0IE = 0 ;
        }
    }
  }
}

void block_for_60_sec(){
    blocked = 1; 
    blocked_seconds = 60 ;
    
    TMR0 = 0 ;
    TMR0IF = 0 ;

    // nRBPU=0 (pull-ups ON), T0CS=0, PSA=0 (prescaler to TMR0), PS=111 (1:256)
    OPTION_REG = 0x07 ;

    TMR0IE = 1 ;  // enable timer0 interrupt LAST
}

void menu(){
    unsigned char result;
    while((result = login())!= 1 && attempts){
   }
    if(attempts == 0){
        attempts = 3 ;
        block_for_60_sec();
        return ;
    }
    logs();
 }
