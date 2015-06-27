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
// TWI rtc address
#define TWI_RTC_ADDRESS 0x68
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
    unsigned char data, buffer[9], i;
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        for (i = 0; i <= 0x6; i++) {
            data = twi_read_byte(0x68, i);
            util_int_to_hex((unsigned)data, buffer);
            put_string(buffer);
            put_string(" ");
        }
        put_string("\n");
    }
}
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
}
// main function
extern void main(void) {
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    put_string("Setup rtc\n");
    // halt rtc counter
    twi_write_byte(TWI_RTC_ADDRESS, 0x0, 0x80);
    // 00 minutes
    twi_write_byte(TWI_RTC_ADDRESS, 0x1, 0x0);
    // 00 hours
    twi_write_byte(TWI_RTC_ADDRESS, 0x2, 0x0);
    // 01 date
    twi_write_byte(TWI_RTC_ADDRESS, 0x3, 0x1);
    // 01 day
    twi_write_byte(TWI_RTC_ADDRESS, 0x4, 0x1);
    // 01 month
    twi_write_byte(TWI_RTC_ADDRESS, 0x5, 0x1);
    // 01 year
    twi_write_byte(TWI_RTC_ADDRESS, 0x6, 0x1);
    // enable rtc counter
    twi_write_byte(TWI_RTC_ADDRESS, 0x0, 0x0);
    put_string("Start rtc\n");
    twi_write_byte(TWI_RTC_ADDRESS, 0x7, 0x13);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x8000;
    // setup rtt interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
