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
// Low level initing Soc
extern void low_level_init(void) {
    volatile int i;
    char string[10];
    // Enable led
    AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    // enable dbgu
    dbgu_init(48000000, 115200);
    // put first string
    put_string("\n");
    put_string("Start on 48MHz clock\n");
    // wait for stable
    for (i = 1; i <= 120000; i++);
    // change clock to main clock
    mck_en();
    // wait for stable
    for (i = 1; i <= 120000; i++);
    // reinit dbgu on 16MHz msck
    dbgu_init(16000000, 115200);
    // put second string
    put_string("\n");
    put_string("Printing on 16MHz clock\n");
    // Infinity loop
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
}
