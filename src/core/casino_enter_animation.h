#include "casino_pay_01.h"
#include "casino_pay_02.h"

void animate_casino_enter(lcd_t dev) {


    //version02

    lcd_pixmap(&dev, 0, 159, 0, 127, pay_01_bitmap);
    busy_wait_us(500000); //delay
    lcd_pixmap(&dev, 0, 159, 0, 127, pay_02_bitmap);
    busy_wait_us(700000); //delay


    }