/**
 * Main code
 */
#include <AT91RM9200.h>
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
    aic_disable_irq(AT91C_ID_IRQ3);
        if (AT91C_BASE_ST->ST_IMR & AT91C_ST_PITS)
            AT91C_BASE_ST->ST_IDR = AT91C_ST_PITS;
        else
            AT91C_BASE_ST->ST_IER = AT91C_ST_PITS;
    aic_enable_irq(AT91C_ID_IRQ3);
}
// system timer handler
extern void st_handler(void) {
    int status = AT91C_BASE_ST->ST_SR & AT91C_BASE_ST->ST_IMR;
    // handler of pit
    if (status & AT91C_ST_PITS) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB0;
    }
    // handler of rtt - rttinc
    if (status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
    }
    // handler of rtt - alms
    if (status & AT91C_ST_ALMS) {
        put_string("Simple message\n");
        AT91C_BASE_ST->ST_RTAR += 40;
    }
}
// main function
extern void main(void) {
    // enable IRQ3 i/o pins
    AT91C_BASE_PIOA->PIO_BSR |= AT91C_PA23_IRQ3;
    AT91C_BASE_PIOA->PIO_PDR |= AT91C_PA23_IRQ3;
    // setup IRQ3 interrupt
    aic_configure_irq(AT91C_ID_IRQ3, AT91C_AIC_PRIOR_HIGHEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_irq3_handler);
    // setup ST interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_st_handler);
    // enable IRQ3 interrupt
    aic_enable_irq(AT91C_ID_IRQ3);
    // enable ST interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup pit on 1Hz blink
    AT91C_BASE_ST->ST_PIMR = 0x7fff;
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    // send message after 20 seconds
    AT91C_BASE_ST->ST_RTAR = 40;
    // setup rtt interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
    // setup pit interrupt flag
    AT91C_BASE_ST->ST_IDR = AT91C_ST_PITS;
    // setup rtt alarm interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_ALMS;
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
        put_string("Infinity loop\n");
    }
}
