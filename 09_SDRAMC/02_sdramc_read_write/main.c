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
    int temp;
    char buffer[9];
    // base address of sdram region
    volatile unsigned int * pt_mem_area = (unsigned int *) 0x20000000;
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // temp data is 0x12345678
    temp = 0x12345678;
    util_int_to_hex(temp, buffer);
    put_string("Temp data: ");
    put_string(buffer);
    put_string("\n");
    put_string("Writing temp to memory address 0x00000000\n");
    pt_mem_area[0x00000000] = temp;
    put_string("Reading memory address 0x00000000\n");
    util_int_to_hex(pt_mem_area[0x00000000], buffer);
    put_string("Memory-0x00000000: ");
    put_string(buffer);
    put_string("\n");
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
