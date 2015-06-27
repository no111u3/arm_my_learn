/**
 * Main code
 */
#include <AT91RM9200.h>
// prototypes
extern void aic_asm_irq3_handler(void);
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
// main function
extern void main(void) {
    // enable IRQ3 i/o pins
    AT91C_BASE_PIOA->PIO_BSR |= AT91C_PA23_IRQ3;
    AT91C_BASE_PIOA->PIO_PDR |= AT91C_PA23_IRQ3;
    // setup IRQ3 interrupt
    aic_configure_irq(AT91C_ID_IRQ3, AT91C_AIC_PRIOR_HIGHEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_irq3_handler);
    // enable IRQ3 interrupt
    aic_enable_irq(AT91C_ID_IRQ3);
    // Infinity loop
    while (1) {
        wait();
        AT91C_BASE_PIOB->PIO_SODR = AT91C_PIO_PB27;
        wait();
        AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    }
}
