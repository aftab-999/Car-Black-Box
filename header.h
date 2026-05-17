/* 
 * File:   header.h
 * Author: aftab
 *
 * Created on 14 May, 2026, 1:47 PM
 */

#ifndef HEADER_H
#define	HEADER_H

#include<xc.h>

#define _XTAL_FREQ 20000000

//i2c definations
#define FOSC 20000000
#define SLAVE_WRITE 0xD0
#define SLAVE_READ 0xD1
#define EEPROM_SLAVE_WRITE 0xA0
#define EEPROM_SLAVE_READ 0xA1

#define SEC_ADDR  0x00 
#define MIN_ADDR  0x01
#define HOUR_ADDR 0x02
#define DAY_ADDR  0x03
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06

// clcd definations 
#define HI 1
#define LOW 0 

#define SIZE 17
#define LINE1(x) (0x80 + x)
#define LINE2(x) (0xC0 + x) 

#define CLCD_DATA_PORT_DDR TRISD
#define CLCD_RS_DDR        TRISE2
#define CLCD_EN_DDR        TRISE1

#define CLCD_DATA_PORT     PORTD
#define CLCD_RS            RE2
#define CLCD_EN            RE1

#define INST_MODE          0
#define DATA_MODE          1

#define EIGHT_BIT_MODE     0x33
#define CLR_DISPLAY        0x01
#define RETURN_HOME        0x02
#define ENTRY_MODE         0x06
#define DISP_ON_OFF        0x0C
#define TWO_LINES_5x8_BIT_MODE   0x38

// Variables & arrays & array of pointers (GLOBAL)
extern unsigned char time[9];
extern unsigned char *event[9];
extern unsigned char curr_gear;
extern unsigned char str[];
extern unsigned char str2[];
extern unsigned char password[4]; 
extern unsigned char failed[];
extern unsigned char success[];
extern unsigned char attempts;
extern unsigned char eeprom_str[10];
extern unsigned char memory_index; // memory_index E0 E1 E2 E3 E4 E5 E6 E7 E8 E9
extern volatile unsigned char blocked;
extern volatile unsigned char blocked_seconds;

// dashboard
void dashboard() ;

// i2c
void init_i2c(unsigned int long baud);
void i2c_wait_for_idle();
void i2c_start();
void i2c_stop();
void i2c_repeat_strt();
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ack);
void write_external_eeprom(unsigned char addr, unsigned char data);
unsigned char read_external_eeprom(unsigned char addr);

// login 
unsigned char login();

// logs
unsigned char* read_eeprom(unsigned char  , unsigned char , unsigned char );
unsigned char dec_to_bcd(unsigned char );
void enter_view_log();
void enter_clear_log();
void uart_transmit(unsigned char );
void enter_download_log();
void set_time_log();
void change_pass_log();
void logs();

// main
void read_data();
void store_event(unsigned char *str);

//rtc 
unsigned char read_ds1307(unsigned char );
void write_ds1307(unsigned char , unsigned char );
unsigned char bcd_to_decimal(unsigned char );
void init_rtc();

// menu
void menu();
void block_for_60_sec();
void __interrupt() isr();

// timer 
void init_timer0() ;

// uart 
void init_uart(unsigned long int );

// clcd

void clcd_write(unsigned char  , unsigned char );
void init_disp_controller();
void clcd_print(unsigned char* str , unsigned char ) ;
void clcd_putch(unsigned char , unsigned char );
void init_clcd();

// adc
void init_adc();
unsigned short int read_adc(unsigned char);

#endif	/* HEADER_H */

