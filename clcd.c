#include<xc.h>
#include"header.h"

void clcd_write(unsigned char byte , unsigned char mode){
    CLCD_RS = mode ; 
    CLCD_DATA_PORT = byte ;

    CLCD_EN = HI ;    // when EN goes from HIGH to LOW it when CLCD captures the data
    __delay_us(100) ;
    CLCD_EN = LOW ;
    __delay_us(4100) ;
}
void init_disp_controller(){
    __delay_ms(15);
    clcd_write(EIGHT_BIT_MODE , INST_MODE);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE , INST_MODE);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE , INST_MODE);
    __delay_us(100);
    clcd_write(TWO_LINES_5x8_BIT_MODE , INST_MODE);
    __delay_us(100);
    clcd_write(DISP_ON_OFF , INST_MODE);
    __delay_us(100);
    clcd_write(CLR_DISPLAY , INST_MODE);
    __delay_us(100);
    clcd_write(ENTRY_MODE , INST_MODE);
    __delay_us(100);
}
void clcd_print(unsigned char *str , unsigned char addr){
    clcd_write(addr , INST_MODE);

    while(*str){
        clcd_write(*str, DATA_MODE);
        str++ ;
    }
}
void clcd_putch(unsigned char data , unsigned char addr){
    clcd_write(addr , INST_MODE);
    clcd_write(data , DATA_MODE);
}
void init_clcd(){
    CLCD_DATA_PORT_DDR = 0x00 ; // setting TRISD as OUTPUT 
    CLCD_RS_DDR        = 0 ;
    CLCD_EN_DDR        = 0 ;

    init_disp_controller();

}