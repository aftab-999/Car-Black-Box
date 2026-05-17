#include<xc.h>
#include "header.h"

void init_i2c(unsigned int long baud){
    SSPEN = 1 ; // Enable serial port and configures SDA and SCL pins as serial ports

    SSPM3 = 1; // enable I2C Master mode

    SSPADD =  (FOSC / (baud*4)) - 1; // it acts as baud generator or holds slave device address
}

void i2c_wait_for_idle(){
    while(R_nW || (SSPCON2 & 0x1F));
}
void i2c_start(){
    i2c_wait_for_idle();
    SEN = 1 ; //  Initiate Start condition on SDA and SCL pins
}
void i2c_stop(){
    i2c_wait_for_idle();
    PEN = 1 ; //  Initiate Stop condition on SDA and SCL pins
}
void i2c_repeat_strt(){
    i2c_wait_for_idle();
    RSEN = 1 ;
}
void i2c_write(unsigned char data)
{
    i2c_wait_for_idle();

    SSPBUF = data; // put the data in buffer

    while(BF); // while buffer is full it keeps on waiting for buffer to be empty 

    while(SSPSTATbits.R_nW); // wait for the buffer to be empty

    while(ACKSTAT); // wait for ack
}
unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;

    i2c_wait_for_idle();

    RCEN = 1;

    while(!BF);

    data = SSPBUF;

    i2c_wait_for_idle();

    ACKDT = (ack == 1) ? 0 : 1;

    ACKEN = 1;

    while(ACKEN);

    return data;
}

void write_external_eeprom(unsigned char addr, unsigned char data) {
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE); // Send EEPROM address + W bit (0xA0)
    i2c_write(addr);               // Send the internal memory address
    i2c_write(data);               // Send the data byte
    i2c_stop();
    
    // CRITICAL: External EEPROMs need about 5ms to physically write data to memory!
    // If I try to read or write again immediately, the chip will ignore you.
    __delay_ms(10); 
}

unsigned char read_external_eeprom(unsigned char addr) {
    unsigned char data;
    
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE); // Dummy write to set address pointer
    i2c_write(addr);               // Send internal memory address
    
    i2c_repeat_strt();             // Repeated Start
    i2c_write(EEPROM_SLAVE_READ);  // Send EEPROM address + R bit (0xA1)
    
    data = i2c_read(0);            // Read data and send NACK (0) to signal end of read
    i2c_stop();
    
    return data;
}