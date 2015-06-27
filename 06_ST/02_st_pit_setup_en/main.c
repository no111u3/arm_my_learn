/**
 * Main code
 */
#include <AT91RM9200.h>
// global vars
volatile int msg_flag;
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
    int status = AT91C_BASE_ST->ST_SR;
    // handler of pit
    if (status & AT91C_ST_PITS) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB0;
        msg_flag = 1;
    }
}
// main function
extern void main(void) {
    // setup vars
    msg_flag = 0;
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
    // setup pit on 1Hz blink
    AT91C_BASE_ST->ST_PIMR = 0x7fff;
    // Infinity loop
    put_string("Infinity loop\n");
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        if (msg_flag) {
            put_string("Change led voltage\n");
            msg_flag = 0;
        }
    }
}
