#include<xc.h>
#include "header.h"

void dashboard(){

    // display header on Line 1
    clcd_print("TIME    GR  SP", LINE1(0));

    // get_time 
    unsigned char hour = read_ds1307(HOUR_ADDR);
    hour = bcd_to_decimal(hour);
    time[0] = (hour/10) + '0' ;
    time[1] = (hour%10) + '0' ;
    time[2] = ':';
    unsigned char min = read_ds1307(MIN_ADDR);
    min = bcd_to_decimal(min);
    time[3] = (min/10) + '0' ;
    time[4] = (min%10) + '0' ;
    time[5] = ':';
    unsigned char sec = read_ds1307(SEC_ADDR);
    sec = bcd_to_decimal(sec);
    time[6] = (sec/10) + '0' ;
    time[7] = (sec%10) + '0' ;

    time[8] = '\0' ;
    
    // display_time on Line 2
    clcd_print(time , LINE2(0));

    //display current gear
    clcd_putch(event[curr_gear][0], LINE2(9));
    clcd_putch(event[curr_gear][1], LINE2(10));

    // display current speed 
    unsigned short int speed = read_adc(0) ;
    speed = speed / 10 ;  // scale ADC to 0-99 range

    clcd_putch((speed/10)+'0' , LINE2(12));
    clcd_putch((speed%10)+'0' , LINE2(13));
}
