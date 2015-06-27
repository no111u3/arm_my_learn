/**
 * Main code
 */
#include <AT91RM9200.h>
#include <twi/twi_re_tr.c>
#include <util/util_int_str.c>
// global vars
// Memory size in MB
#define MEM_SIZE_M 64
// Memory begin
#define MEM_BEGIN 0x00000000
// Memory end
#define MEM_END MEM_BEGIN + (MEM_SIZE_M << 20) - 1
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
    unsigned char data, buffer[9], i;
    
    put_string("exteral rtc read\n");
    
    put_string("external rtc read all bytes\n");
    for (i = 0; i <= 0x3f; i++) {
        util_int_to_hex(i, buffer);
        put_string("Address: 0x");
        put_string(buffer);
        put_string(" data: 0x");
        data = twi_read_byte(0x68, i);
        util_int_to_hex((unsigned)data, buffer);
        put_string(buffer);
        put_string("\n");
    }
    put_string("external rtc write all bytes\n");
    for (i = 0; i <= 0x3f; i++) {
        twi_write_byte(0x68, i, i);
    }
    put_string("external rtc read all bytes\n");
    for (i = 0; i <= 0x3f; i++) {
        util_int_to_hex(i, buffer);
        put_string("Address: 0x");
        put_string(buffer);
        put_string(" data: 0x");
        data = twi_read_byte(0x68, i);
        util_int_to_hex((unsigned)data, buffer);
        put_string(buffer);
        put_string("\n");
    }
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
