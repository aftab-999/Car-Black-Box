#include<xc.h>
#include "header.h"

unsigned char time[9] = {0};
unsigned char *event[9] = {"ON","GN","G1","G2","G3","G4","G5","GR","C "};
unsigned char curr_gear = 0 ;
unsigned char str[] = "Enter Password";
unsigned char str2[] = "Attempt Remain ";
unsigned char password[4] = "0011"; 
unsigned char failed[] = "Login Failed!";
unsigned char success[] = "Login Success!";
unsigned char attempts = 3;
unsigned char eeprom_str[10];
unsigned char memory_index ; // memory_index E0 E1 E2 E3 E4 E5 E6 E7 E8 E9
volatile unsigned char blocked = 0 ;
volatile unsigned char blocked_seconds = 0 ;

#pragma config WDTE = OFF
#pragma config LVP = OFF
#pragma config FOSC = HS
#define _XTAL_FREQ 20000000

static void init_config(){
    init_adc();       // MUST be first — sets ADCON1 to make PORTE digital for LCD
    init_i2c(100000);
    init_rtc();
    init_clcd();
    init_timer0();
    init_uart(9600);

    TRISB = TRISB | 0x3F ;

    // Clear ALL stray interrupt flags before enabling GIE
    TMR0IF = 0 ;
    INTF = 0 ;
    RBIF = 0 ;
    SSPIF = 0 ;
    TMR1IF = 0 ;
    ADIF = 0 ;

    // Enable interrupts LAST
    GIE = 1 ;
    PEIE = 1 ;
    
}
void read_data(){
    unsigned char hour = read_ds1307(HOUR_ADDR);
    hour = bcd_to_decimal(hour);
    eeprom_str[0] = (hour/10) + '0' ;
    eeprom_str[1] = (hour%10) + '0' ;
    
    unsigned char min = read_ds1307(MIN_ADDR);
    min = bcd_to_decimal(min);
    eeprom_str[2] = (min/10) + '0' ;
    eeprom_str[3] = (min%10) + '0' ;
    
    unsigned char sec = read_ds1307(SEC_ADDR);
    sec = bcd_to_decimal(sec);
    eeprom_str[4] = (sec/10) + '0' ;
    eeprom_str[5] = (sec%10) + '0' ;

    eeprom_str[6] = event[curr_gear][0];
    eeprom_str[7] = event[curr_gear][1];

    // read current speed from ADC
    unsigned short int speed = read_adc(0) ;
    speed = speed / 10 ;  // scale ADC to 0-99

    eeprom_str[8] = (speed/10)+'0';
    eeprom_str[9] = (speed%10)+'0';
}
void store_event(unsigned char *str){
        unsigned int addr = memory_index * 10 ;
        for(unsigned char i=0;i<10;i++){
            write_external_eeprom(addr+i , str[i]);
            __delay_ms(5);
        }
        memory_index++ ;
        if(memory_index >= 10){
            memory_index = 0 ;
        }
        write_external_eeprom(100 , memory_index);
        __delay_ms(5);
   }
void main(void){
    unsigned char once = 1 ;
    unsigned char once2 = 1 ;
    unsigned char once3 = 1 ;
    unsigned char once4 = 1 ;
    unsigned char prev_blocked_sec = 0xFF ;  // track displayed seconds to avoid redraw
 
    init_config();
    
    memory_index = read_external_eeprom(100); // reading memory index once before starting while(1)
    if(memory_index >= 10){
        memory_index = 0 ;
        }
    if(read_external_eeprom(200) != 0xAA){
        write_ds1307(0x17 , HOUR_ADDR);
        write_ds1307(0x50 , MIN_ADDR);
        write_ds1307(0x00 , SEC_ADDR);
           
        write_external_eeprom(200 , 0xAA);
        __delay_ms(5);
    } else {
        // Load saved password from EEPROM
        for(unsigned char i=0; i<4; i++){
            unsigned char ch = read_external_eeprom(150 + i);
            if(ch == '0' || ch == '1'){
                password[i] = ch;
            }
        }
    }

    // Wait for power-up stabilization
    __delay_ms(500);

    while(1){
          if(blocked == 1){
            // Only redraw when seconds change (no flicker)
            if(blocked_seconds != prev_blocked_sec){
                // show header only on first display
                if(prev_blocked_sec == 0xFF){
                    clcd_write(CLR_DISPLAY , INST_MODE);
                    clcd_print("BLOCKED FOR " , LINE1(2));
                }
                prev_blocked_sec = blocked_seconds ;
                
                clcd_putch((blocked_seconds/10) + '0', LINE2(4));
                clcd_putch((blocked_seconds%10) + '0', LINE2(5));
                clcd_print("SEC" , LINE2(7));
            }
            continue ;
           }
          prev_blocked_sec = 0xFF ;  // reset tracker when unblocked

        if(RB0 == 0 && once){    // Collision event
            once = 0 ;
            curr_gear = 8 ;

            read_data();
            store_event(eeprom_str);
            
        }else if(RB1 == 0 && once2){  // Gear UP
            once2 = 0 ;
            if(curr_gear < 7){
                curr_gear++;
            }
            read_data();
            store_event(eeprom_str);
            
        }else if(RB2 == 0 && once3){  // Gear DOWN
            once3 = 0 ;
            if(curr_gear > 1){
                curr_gear--;
            }
            read_data();
            store_event(eeprom_str);
        }
        else if(RB3 == 0 && once4){   // Enter Menu
          once4 = 0 ;
          clcd_write(CLR_DISPLAY , INST_MODE);
          menu();
          clcd_write(CLR_DISPLAY , INST_MODE);
        }
        if(RB0 == 1){
            once = 1;
        }
        if(RB1 == 1){
            once2 = 1;
        }
        if(RB2 == 1){
            once3 = 1;
        }
        if(RB3 == 1){
            once4 = 1; 
        }
        if(blocked == 0){
            dashboard();
        }
    }
}