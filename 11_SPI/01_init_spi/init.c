/**
 * Init MPU
*/
#include <AT91RM9200.h>
#include <dbgu/dbgu_init.c>
#include <dbgu/dbgu_re_tr.c>
#include <pmc/pmc_init.c>
#include <pmc/pmc_periph.c>
#include <aic/aic_init.c>
#include <aic/aic_int_handlers.c>
#include <ebi/ebi_init.c>
#include <sdramc/sdramc_init.c>
#include <spi/spi_init.c>
#include <spi/spi_cs.c>
// the simple waiting function
void delay(int tic) {
    volatile int i;
    for (i = 1; i <= tic; i++);
}
// Low level initing Soc
extern void low_level_init(void) {
    // enable led 0
    AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB27;
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    // enable led 1
    AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB0;
    AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB0;
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB0;
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
    aic_init(AT91C_AIC_BRANCH_OPCODE, aic_default,
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
    // initing ebi
    ebi_init();
    // initing sdramc
    sdramc_init();
    // initing spi
    spi_init('m');
    // spi - setup #0 cs, 8bits, 4MHz, 0 delay, 0 mode
    spi_cs_set(0, 8, 4, 0, 0);
}
