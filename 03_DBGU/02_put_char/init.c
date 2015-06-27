/**
 * Init MPU
*/
#include <AT91RM9200.h>
#include "dbgu_init.c"
#include "dbgu_re_tr.c"
// the down-tic waiting function
void wait(void) {
    volatile int i;
    static int end = 1;
    for (i = 1; i <= end; i++);
    end++;
}
// Low level initing Soc
extern void low_level_init(void) {
    // Enable led
    AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    // enable dbgu
    dbgu_init();
    put_char('S');
    // Infinity loop
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
}
