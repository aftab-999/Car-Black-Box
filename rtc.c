#include<xc.h>
#include "header.h"

unsigned char read_ds1307(unsigned char addr){
    unsigned char data ;
    i2c_start();
    i2c_write(SLAVE_WRITE); // 0xD0
    i2c_write(addr);        // pass the address in which we have to read
    i2c_repeat_strt();
    i2c_write(SLAVE_READ);  // 0xD1 (now again we have to pass unique id + R/W Bit )
    data = i2c_read(0); //send 0 as an argument means send Ack after reading the data
   
    i2c_stop();

    return data ;
}
void write_ds1307(unsigned char data , unsigned char addr){
    i2c_start();
    i2c_write(SLAVE_WRITE);// send unique id of ds1307 + W bit
    i2c_write(addr);// send address 
    i2c_write(data);// send data
    i2c_stop();
}
unsigned char bcd_to_decimal(unsigned char num){
    return ((num >> 4) * 10) + (num & 0x0F) ;
}
void init_rtc(){
    unsigned char data; 
    data = read_ds1307(SEC_ADDR);
    data = data & 0x7F ;  // CH Bit = 0 ;
    write_ds1307(data , SEC_ADDR);
}