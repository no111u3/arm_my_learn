/**
 * Init MPU
*/
#include <AT91RM9200.h>
#include <dbgu/dbgu_init.c>
#include <dbgu/dbgu_re_tr.c>
#include <pmc/pmc_init.c>
#include <aic/aic_init.c>
#include <aic/aic_int_handlers.c>
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
    int *pointer;
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
    put_string("Running on 16MHz clock\n");
    // init aic
    aic_init(aic_default, aic_default,
             aic_default, aic_spirious_int, 0);
    // perform 8 end of interrupt command to make sure AIC will not lock out nIRQ
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    aic_acknowledge_irq();
    // setup basic exception vectors
    aic_set_exception_vector((unsigned int *)0x0C, aic_fetch_abort);
    aic_set_exception_vector((unsigned int *)0x10, aic_data_abort);
    aic_set_exception_vector((unsigned int *)0x4, aic_undefined);
    // generate data abort interrupt
    // pointer on undefined data region
    pointer = (unsigned int *)0x90000001;
    // write to this
    *pointer = 1;
    // Infinity loop
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
}
