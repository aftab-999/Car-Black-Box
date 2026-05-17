#include<xc.h>
#include"header.h"
 
unsigned char* read_eeprom(unsigned char address , unsigned char curr_index, unsigned char clcd_line){
    static unsigned char array[17]; 
    array[0] = curr_index + '0';  // display index as ASCII digit
    array[1] = ' ';
    array[2] = read_external_eeprom(address + 0);
    array[3] = read_external_eeprom(address + 1);
    array[4] = ':';
    array[5] = read_external_eeprom(address + 2);
    array[6] = read_external_eeprom(address + 3);
    array[7] = ':';
    array[8] = read_external_eeprom(address + 4);
    array[9] = read_external_eeprom(address + 5);
    array[10] = ' ';
    array[11] = read_external_eeprom(address + 6);
    array[12] = read_external_eeprom(address + 7);
    array[13] = ' ';
    array[14] = read_external_eeprom(address + 8);
    array[15] = read_external_eeprom(address + 9);
    array[16] = '\0';
    
    clcd_print(array , clcd_line);
    return array ;
}

unsigned char dec_to_bcd(unsigned char data){
    return ((data / 10) << 4) | (data % 10) ;
}

void enter_view_log(){
    unsigned char curr_index = 0 ;

    unsigned char address[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90} ;
    
    clcd_write(CLR_DISPLAY, INST_MODE);
    read_eeprom(address[curr_index], curr_index, LINE1(0));
    if(curr_index + 1 <= 9){
        read_eeprom(address[curr_index+1], curr_index+1, LINE2(0));
    }

    unsigned long int i = 1000000 ;
    unsigned char once4 = 1 ;
    unsigned char once5 = 1 ;

    while(RB3 == 0 || RB4 == 0 || RB5 == 0); // Wait for button release

    while(i--){
        if(RB3 == 0 && once4 && curr_index >= 1){   // Scroll UP
            __delay_ms(50);
            if(RB3 == 0){
                once4 = 0 ;
                i = 1000000 ;
                curr_index-- ;
                clcd_write(CLR_DISPLAY, INST_MODE);
                read_eeprom(address[curr_index], curr_index, LINE1(0));
                if(curr_index + 1 <= 9){
                    read_eeprom(address[curr_index+1], curr_index+1, LINE2(0));
                }
            }
        }else if(RB4 == 0 && once5 && curr_index < 8){  // Scroll DOWN
            __delay_ms(50);
            if(RB4 == 0){
                once5 = 0 ;
                i = 1000000 ;
                curr_index++ ;
                clcd_write(CLR_DISPLAY, INST_MODE);
                read_eeprom(address[curr_index], curr_index, LINE1(0));
                if(curr_index + 1 <= 9){
                    read_eeprom(address[curr_index+1], curr_index+1, LINE2(0));
                }
            }
        }
        if(RB3 == 0){
            once4 = 1 ;
        }
        if(RB4 == 0){
            once5 = 1 ;
        }
    }
}

void enter_clear_log(){
    unsigned char curr_idx = 0 ;

    while(curr_idx <= 9){
        for(unsigned char i=0; i<10; i++){    // 10 bytes per event
            unsigned char address = curr_idx * 10 ;
            write_external_eeprom(address+i, '\0');
        }
        curr_idx++ ;
    }
    memory_index = 0 ;
    write_external_eeprom(100, 0);
    clcd_write(CLR_DISPLAY, INST_MODE);
    clcd_print("DATA CLEARED !", LINE2(0));
    __delay_ms(1000);
}

void uart_transmit(unsigned char data){
    while(TXIF == 0); // whenTXIF becomes 1  means that buffer has sent the data to the reciever
    TXREG = data ;  // loading the data in buffer
}

void enter_download_log(){
    unsigned char curr_idx = 0 ;
    
    for(unsigned char i=0; i<=9; i++){ // loop run for 10 events
        unsigned char address = curr_idx * 10;
        unsigned char* array = read_eeprom(address, curr_idx, LINE2(0));
        curr_idx++;
        while(*array){  // run till you get the null terminator
            uart_transmit(*array++);// increment pointer after sending the data 
        }
        uart_transmit('\r'); // carry return 
        uart_transmit('\n'); // new line 
    }    
}

void set_time_log(){
    unsigned char arr[9] = "00:00:00";
    unsigned char i = 0; 
    unsigned char once4 = 1 ;
    unsigned char once5 = 1 ;
    unsigned char once6 = 1 ;
    arr[8] = '\0';
    clcd_write(CLR_DISPLAY , INST_MODE);
    clcd_print("SET TIME", LINE1(4));
    clcd_print(arr , LINE2(3));
    clcd_write(0x0F , INST_MODE) ;   // cursor ON
    clcd_write(0xC3 + i, INST_MODE); // move cursor to first digit

    unsigned char two_digit_number = 0 ;
    unsigned int timeout = 50000 ;

    while(RB3 == 0 || RB4 == 0 || RB5 == 0); // Wait for button release

    while(arr[i] != '\0' && timeout--){
        clcd_write(0xC3 + i, INST_MODE);
        if(arr[i] != ':' && arr[i] != ' ' && arr[i] != '\0'){
            if(RB3 == 0 && once4){           // Decrement digit
                __delay_ms(50);
                if(RB3 == 0){
                    once4 = 0 ;
                    timeout = 2500 ;
                    if(arr[i] > '0'){
                        arr[i]-- ;
                    }
                    clcd_putch(arr[i], LINE2(3+i));
                }
            }
            if(RB4 == 0 && once5){           // Increment digit
                __delay_ms(50);
                if(RB4 == 0){
                    once5 = 0 ;
                    timeout = 2500 ;
                    if(arr[i] < '9'){
                        arr[i]++ ;
                    }
                    clcd_putch(arr[i], LINE2(3+i));
                }
            }
            if(RB5 == 0 && once6){           // Confirm digit & move next
                __delay_ms(50);
                if(RB5 == 0){
                    once6 = 0;
                    timeout = 2500 ;
                
                    two_digit_number = two_digit_number * 10 + (arr[i] - '0');  // convert ASCII to number
                    if(two_digit_number > 9){
                        // We have both digits, write to RTC
                        unsigned char addr = 0 ;
                        if(i == 1){
                            addr = HOUR_ADDR ;
                        }else if(i == 4){
                            addr = MIN_ADDR ;
                        }else if(i == 7){
                            addr = SEC_ADDR ;
                        }
                        write_ds1307(dec_to_bcd(two_digit_number), addr);
                        two_digit_number = 0 ;
                    }
                    i++ ;
                }
            }
            if(RB3 == 1){
                once4 = 1;
            }
            if(RB4 == 1){
                once5 = 1 ;
            }
            if(RB5 == 1){
                once6 = 1 ;
            }
        }else{
            i++ ;  // skip ':' characters
        }
    }
    clcd_write(0x0C , INST_MODE);  // cursor OFF
    clcd_write(CLR_DISPLAY , INST_MODE);
    clcd_print("TIME SET !", LINE2(3));
}

void change_pass_log(){
    unsigned char once4 = 1 ;
    unsigned char once5 = 1 ;
    unsigned char count = 0 ;
    unsigned long int timeout = 1000000 ;
    clcd_write(CLR_DISPLAY , INST_MODE);
    clcd_print("NEW PASSWORD", LINE1(2));

    while(RB3 == 0 || RB4 == 0 || RB5 == 0); // Wait for button release

    while(count < 4 && timeout--){
        if(RB3 == 0 && once4){
            __delay_ms(50);
            if(RB3 == 0){
                once4 = 0;
                timeout = 1000000;
                password[count] = '0';
                write_external_eeprom(150 + count, password[count]);
                __delay_ms(5);
                clcd_putch('*', LINE2(6+count));
                count++ ;
            }
        }
        if(RB4 == 0 && once5){
            __delay_ms(50);
            if(RB4 == 0){
                once5 = 0;
                timeout = 1000000;
                password[count] = '1';
                write_external_eeprom(150 + count, password[count]);
                __delay_ms(5);
                clcd_putch('*', LINE2(6+count));
                count++ ;
            }
        }
        if(RB3 == 1){
            once4 = 1;
        }
        if(RB4 == 1){
            once5 = 1;
        }
    } 
    clcd_write(CLR_DISPLAY , INST_MODE);
    clcd_print("PASS CHANGED !", LINE2(1));
}

void logs(){
    unsigned char *all_strings[] = {"View Logs","Clear Logs","Download Logs","Set Time","Change Pass"};
    unsigned char top_index = 0 ;     // index of item shown on Line 1
    unsigned char arrow_line = 0 ;    // 0 = arrow on Line1, 1 = arrow on Line2
    unsigned char once4 = 1 ;
    unsigned char once5 = 1 ;
    unsigned char once6 = 1 ;
    unsigned long int i = 1000000;
    unsigned char need_refresh = 1;

    while(RB3 == 0 || RB4 == 0 || RB5 == 0); // Wait for button release

    while(i--){
        // Refresh display when needed
        if(need_refresh){
            need_refresh = 0;
            clcd_write(CLR_DISPLAY , INST_MODE);
            // Line 1: show top_index item
            if(arrow_line == 0){
                clcd_putch('>', LINE1(0));
            }
            clcd_print(all_strings[top_index], LINE1(2));
            // Line 2: show top_index+1 item
            if(top_index + 1 <= 4){
                if(arrow_line == 1){
                    clcd_putch('>', LINE2(0));
                }
                clcd_print(all_strings[top_index+1], LINE2(2));
            }
        }

        // RB3 = Scroll UP
        if(RB3 == 0 && once4){
            __delay_ms(50);
            if(RB3 == 0){
                once4 = 0 ;
                i = 1000000 ;
                if(arrow_line == 1){
                    arrow_line = 0 ;      // move arrow from Line2 to Line1
                }else if(top_index > 0){
                    top_index-- ;         // scroll list up
                }
                need_refresh = 1;
            }
        }

        // RB4 = Scroll DOWN
        if(RB4 == 0 && once5){
            __delay_ms(50);
            if(RB4 == 0){
                once5 = 0;
                i = 1000000 ;
                if(arrow_line == 0){
                    arrow_line = 1 ;      // move arrow from Line1 to Line2
                }else if(top_index < 3){
                    top_index++ ;         // scroll list down
                }
                need_refresh = 1;
            }
        }

        // RB5 = SELECT / ENTER
        if(RB5 == 0 && once6){
            __delay_ms(50);
            if(RB5 == 0){
                once6 = 0 ;
                i = 1000000 ;
                unsigned char selected = top_index + arrow_line ;
                clcd_write(CLR_DISPLAY , INST_MODE);
                if(selected == 0){
                    enter_view_log() ;
                }else if(selected == 1){
                    enter_clear_log();
                }else if(selected == 2){
                    enter_download_log();
                }else if(selected == 3){
                    set_time_log();
                }else if(selected == 4){
                    change_pass_log();
                }
                need_refresh = 1;  // refresh menu after returning
            }
        }
    
        if(RB3 == 1){
            once4 = 1;
        }
        if(RB4 == 1){
            once5 = 1;
        }
        if(RB5 == 1){
            once6 = 1;
        }
    }
}