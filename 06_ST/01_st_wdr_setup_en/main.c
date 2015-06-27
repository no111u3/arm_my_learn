/**
 * Main code
 */
#include <AT91RM9200.h>
// global vars
volatile int wdr_flag;
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_st_handler(void);
// the down-tic waiting function
extern void wait(void) {
    volatile int i;
    static int end = 1;
    for (i = 1; i <= end; i++);
    end++;
}
// external interrupt 1 handler
extern void eint3_handler(void) {
    static int led2 = 0;
    aic_disable_irq(AT91C_ID_IRQ3);
    if (!led2) {
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB0;
        led2 = 1;
    } else {
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB0;
        led2 = 0;
    }
    aic_enable_irq(AT91C_ID_IRQ3);
}
// system timer handler
extern void st_handler(void) {
    int status = AT91C_BASE_ST->ST_SR;
    // handler of wdr
    if (status & AT91C_ST_WDOVF) {
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB0;
        wdr_flag = 1;
        // enable wdr on 10 sec - test wdr reset
        AT91C_BASE_ST->ST_WDMR = 2560 | AT91C_ST_RSTEN;
        AT91C_BASE_ST->ST_CR = AT91C_ST_WDRST;
    }
}
// main function
extern void main(void) {
    // setup vars
    wdr_flag = 0;
    // enable IRQ3 i/o pins
    AT91C_BASE_PIOA->PIO_BSR |= AT91C_PA23_IRQ3;
    AT91C_BASE_PIOA->PIO_PDR |= AT91C_PA23_IRQ3;
    // setup IRQ3 interrupt
    aic_configure_irq(AT91C_ID_IRQ3, AT91C_AIC_PRIOR_HIGHEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_irq3_handler);
    // setup ST interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_HIGHEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_st_handler);
    // enable IRQ3 interrupt
    aic_enable_irq(AT91C_ID_IRQ3);
    // enable ST interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // enable wdr on 10 sec - test wdr start
    AT91C_BASE_ST->ST_WDMR = 2560;
    AT91C_BASE_ST->ST_CR = AT91C_ST_WDRST;
    AT91C_BASE_ST->ST_IER = AT91C_ST_WDOVF;
    put_string("Wait for wdr overflow\n");
    // Infinity loop
    while (wdr_flag == 0) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
    put_string("Wait for wdr reset\n");
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB0;
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB0;
    }
}
