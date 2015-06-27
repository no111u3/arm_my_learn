/**
 * Main code
 */
#include <AT91RM9200.h>
#include <twi/twi_re_tr.c>
#include <util/util_int_str.c>
#include <util/util_printf.c>
// global vars
// Memory size in MB
#define MEM_SIZE_M 64
// Memory begin
#define MEM_BEGIN 0x00000000
// Memory end
#define MEM_END MEM_BEGIN + (MEM_SIZE_M << 20) - 1
// TWI rtc address
#define TWI_RTC_ADDRESS 0x68
#define TWI_EEPROM_ADDRESS 0x50
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
    unsigned short data, counter;
    volatile unsigned i;
    // test read 1 word
    put_string("Read data\n");
    twi_read(TWI_EEPROM_ADDRESS, 0x0, (char *)&data, 2);
    util_printf("Address: 0x0 Data: 0x%x\n", data);
    // test write 1 word
    put_string("Write data\n");
    put_string("Address: 0x0 Data: 0x55aa\n");
    data = 0x55aa;
    twi_write(TWI_EEPROM_ADDRESS, 0x0, (char *)&data, 2);
    // wait 10ms before data is written into EEPROM
    for (i = 0; i < 1000000; i++);
    // read writed word
    put_string("Read data\n");
    util_printf("Address: 0x0 Data: 0x%x\n", data);
    // Infinity loop
    while (1) {    
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
