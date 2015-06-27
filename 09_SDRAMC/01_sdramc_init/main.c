/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_screen.c>
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
// external interrupt 1 handler
extern void eint3_handler(void) {
    aic_disable_irq(AT91C_ID_IRQ3);
    aic_enable_irq(AT91C_ID_IRQ3);
}
// system interrupt handler
extern void sys_handler(void) {
    int st_status = AT91C_BASE_ST->ST_SR & AT91C_BASE_ST->ST_IMR;
    int rtc_status = AT91C_BASE_RTC->RTC_SR & AT91C_BASE_RTC->RTC_IMR;
    int dbgu_status = AT91C_BASE_DBGU->DBGU_CSR & AT91C_BASE_DBGU->DBGU_IMR;
}
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
}
// main function
extern void main(void) {
    // base address of sdram region
    volatile unsigned int * pt_mem_area = (unsigned int *) 0x20000000;
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
