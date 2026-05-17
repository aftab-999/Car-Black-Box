#include<xc.h>
#include "header.h"

unsigned char login(){
    unsigned char once = 1 ;
    unsigned char once2 = 1 ;
    unsigned char count = 0 ;
    char enter_pass[5] ;
    enter_pass[4] = '\0';
    unsigned char line2_idx = 6 ;
    unsigned long int large_num = 5000000;

    clcd_write(CLR_DISPLAY , INST_MODE);

    if(attempts > 2){
        clcd_print(str , LINE1(0));  // Enter the password 
    }
    else if(attempts <= 2 && attempts >= 1){
        clcd_print(str2 , LINE1(0));
        clcd_putch(attempts + '0' , LINE1(15));
    }
    
    while(RB3 == 0 || RB4 == 0 || RB5 == 0); // Wait for buttons to be released

    while(1){
        if(RB3 == 0 && once){
            __delay_ms(50);
            if(RB3 == 0){
                once = 0 ;
                enter_pass[count] = '0' ;
                clcd_putch('*' , LINE2(line2_idx));
                line2_idx++ ;
                count++ ;
            }
        }
        if(RB4 == 0 && once2){
            __delay_ms(50);
            if(RB4 == 0){
                once2 = 0 ;
                enter_pass[count] = '1' ;
                clcd_putch('*' , LINE2(line2_idx));
                line2_idx++ ;
                count++ ;
            }
        }
        if(RB3 == 1){
            once = 1; 
        }
        if(RB4 == 1){
            once2 = 1;
        }
     large_num--;
     if(count == 4 || large_num == 0) break;
    }

    // Timeout: not all 4 digits entered
    if(count < 4){
        clcd_write(CLR_DISPLAY , INST_MODE);
        clcd_print(failed , LINE2(2));
        attempts--;
        return 0;
    }

    for(unsigned char i=0;i<4;i++){
        if(enter_pass[i] != password[i]){
            clcd_write(CLR_DISPLAY , INST_MODE);
            clcd_print(failed , LINE2(2));
            attempts--;
            return 0;
        }
    }

    // Password matched
    clcd_write(CLR_DISPLAY , INST_MODE);
    clcd_print(success , LINE2(2));
    attempts = 3; // reset attempts on success
    return 1 ;
}