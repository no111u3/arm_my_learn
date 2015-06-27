/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_to_str.c>
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_st_handler(void);
extern void aic_asm_tc_handler(void);
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
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
    if (status & AT91C_TC_COVFS) {
        put_string("TC #0 interrupt overflow\n");
    }
    if (status & AT91C_TC_CPCS) {
    }
}
// main function
extern void main(void) {
    // enable IRQ3 and TC0 i/o pins
    AT91C_BASE_PIOA->PIO_BSR |= AT91C_PA23_IRQ3 | AT91C_PA17_TIOA0 |AT91C_PA18_TIOB0;
    AT91C_BASE_PIOA->PIO_PDR |= AT91C_PA23_IRQ3 | AT91C_PA17_TIOA0 |AT91C_PA18_TIOB0;
    // setup IRQ3 interrupt
    aic_configure_irq(AT91C_ID_IRQ3, AT91C_AIC_PRIOR_HIGHEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_irq3_handler);
    // setup ST interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_st_handler);
    // setup TC0 interrupt
    aic_configure_irq(AT91C_ID_TC0, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_tc_handler);
    // enable IRQ3 interrupt
    aic_enable_irq(AT91C_ID_IRQ3);
    // enable ST interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // enable TC0 interrupt
    aic_enable_irq(AT91C_ID_TC0);
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
    // disable clock counter
    AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS;
    // enable tc0 clock
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_TC0;
    // setup timer counter #0
    AT91C_BASE_TC0->TC_CMR = AT91C_TC_BSWTRG_SET | /* BSWTRG : software trigger set TIOB */
        AT91C_TC_BCPC_TOGGLE | /* BCPC : Register C compare toggle TIOB */
        AT91C_TC_BCPB_TOGGLE | /* BCPB : Register B compare toggle TIOB */
        AT91C_TC_ASWTRG_TOGGLE | /* ASWTRG : software trigger set TIOA */
        AT91C_TC_ACPC_TOGGLE | /* ACPC : Register C compare toggle TIOA */
        AT91C_TC_ACPA_TOGGLE | /* ACPA : Register A compare toggle TIOA */
        AT91C_TC_WAVE | /* WAVE : Waveform mode */
        AT91C_TC_CPCTRG | /* CPCTRG : Register C compare trigger enable */
        AT91C_TC_EEVT_RISING | /* EEVT : XC0 as external event (TIOB=output) */
        AT91C_TC_CPCTRG | 0x4; // TIMER_CLOCK_0 - MCK
    // setup tc0 compare registers A, B, C
    AT91C_BASE_TC0->TC_RA = 0x0;
    AT91C_BASE_TC0->TC_RB = 0x10;
    AT91C_BASE_TC0->TC_RC = 0x20; /* 1kHz PWM generation */
    // enable clock counter and trig the timer
    AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKEN;
    // restart timer
    AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG;
    // enable tc0 overflow & compare with register C interrupt
    AT91C_BASE_TC0->TC_IER = AT91C_TC_CPCS | AT91C_TC_COVFS;
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
