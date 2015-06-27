/**
 * Init MPU
*/
#include <AT91RM9200.h>
#include "dbgu_init.c"
#include "dbgu_re_tr.c"
#include "pmc_init.c"
// the down-tic waiting function
void wait(void) {
    volatile int i;
    static int end = 1;
    for (i = 1; i <= end; i++);
    end++;
}
// the simple waiting function
void delay(int tic) {
    volatile int i;
    for (i = 1; i <= tic; i++);
}
// Low level initing Soc
extern void low_level_init(void) {
    char string[10];
    int i;
    // Enable led
    AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    // enable dbgu
    dbgu_init(48000000, 115200);
    // put first string
    put_string("\n");
    put_string("Start on 48MHz clock\n");
    put_string("Switching to main clock\n");
    // wait for stable
    delay(120000);
    // change clock to main clock
    mck_init(AT91C_CKGR_MOSCEN | (0xff << 8));
    mck_en();
    // wait for stable
    delay(120000);
    // reinit dbgu on 16MHz msck
    dbgu_init(16000000, 115200);
    // put second string
    put_string("\n");
    put_string("Printing on 16MHz clock\n");
    put_string("Switching to slow clock\n");
    // wait for stable
    delay(120000);
    // change clock to slow clock
    slck_en();
    delay(1000);
    for (i = 1; i <= 10; i++) {
        delay(1000);
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        delay(1000);
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
    // Infinity loop
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
}
