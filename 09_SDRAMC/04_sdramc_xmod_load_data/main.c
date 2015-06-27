/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_screen.c>
#include <dbgu/dbgu_xmod.c>
// global vars
volatile int temp;
// base address of sdram region
void * pt_mem_area = (unsigned int *) 0x20000000;
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
    
    if (dbgu_status & AT91C_US_RXRDY) {
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        // disable rtt interrupt flag
        AT91C_BASE_ST->ST_IDR = AT91C_ST_RTTINC;
        temp = dbgu_xmod_recv(pt_mem_area);
    }
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        put_char('C');
    }
}
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
}
// main function
extern void main(void) {
    temp = 0;
    char buffer[9];
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    // setup rtt interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
    // enable RXRDY interrupt in DBGU
    AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
    while(!temp);
    delay(100000);
    util_int_to_hex(temp, buffer);
    if (temp > 0) {
        put_string("\n\nTransfer complete\n");
        put_string("Byte's sended: ");
        put_string(buffer);
        put_string("\n");
    } else {
        put_string("Transfer failed\n");
    }
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
