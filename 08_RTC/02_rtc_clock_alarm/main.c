/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_screen.c>
#include <rtc/rtc_setup.c>
#include <rtc/rtc_alarm.c>
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
    char buffer[9];
    int i;
    int temp_time;
    int temp_date;
    int st_status = AT91C_BASE_ST->ST_SR & AT91C_BASE_ST->ST_IMR;
    int rtc_status = AT91C_BASE_RTC->RTC_SR & AT91C_BASE_RTC->RTC_IMR;
    int dbgu_status = AT91C_BASE_DBGU->DBGU_CSR & AT91C_BASE_DBGU->DBGU_IMR;
    
    if (rtc_status & AT91C_RTC_SEC) {
        temp_time = AT91C_BASE_RTC->RTC_TIMR;
        temp_date = AT91C_BASE_RTC->RTC_CALR;
        AT91C_BASE_RTC->RTC_SCCR = AT91C_RTC_SEC;
        
        util_clear_screen();
        // output century
        util_int_to_hex((temp_date & AT91C_RTC_CENT), buffer);
        put_string(buffer);
        // output year
        util_int_to_hex((temp_date & AT91C_RTC_YEAR) >> 8, buffer);
        put_string(buffer);
        put_string(".");
        // output month
        util_int_to_hex((temp_date & AT91C_RTC_MONTH) >> 16, buffer);
        put_string(buffer);
        put_string(".");
        // output date
        util_int_to_hex((temp_date & AT91C_RTC_DATE) >> 24, buffer);
        put_string(buffer);
        put_string(" ");
        // output hours
        util_int_to_hex((temp_time & AT91C_RTC_HOUR) >> 16, buffer);
        put_string(buffer);
        put_string(":");
        // output minuts
        util_int_to_hex((temp_time & AT91C_RTC_MIN) >> 8, buffer);
        put_string(buffer);
        put_string(":");
        // output seconds
        util_int_to_hex((temp_time & AT91C_RTC_SEC), buffer);
        put_string(buffer);
    }
    if (rtc_status & AT91C_RTC_ALARM) {
        AT91C_BASE_RTC->RTC_SCCR = AT91C_RTC_ALARM;
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        put_string("Alarm\n");
    }
    
    if (dbgu_status & AT91C_US_RXRDY) {
        char flag;
        // disable SEC interrupt in RTC
        AT91C_BASE_RTC->RTC_IDR |= AT91C_RTC_SEC;
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        flag = get_char();
        if (flag == 's') {
            util_clear_screen();
            put_string("set date(d) time(t)");
            flag = get_char();
            util_clear_screen();
            if (flag == 'd')
                rtc_update_date();
            else if (flag == 't')
                rtc_update_time();
        }
        else if (flag == 'a') {
            util_clear_screen();
            put_string("alarm date(d) time(t)");
            flag = get_char();
            if (flag == 'd')
                //rtc_alarm_date();
            if (flag == 't')
                rtc_alarm_time();
        }
        // enable SEC & ALARM interrupts in RTC
        AT91C_BASE_RTC->RTC_IER |= AT91C_RTC_SEC;
        // enable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
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
    // enable SEC interrupt in RTC
    AT91C_BASE_RTC->RTC_IER |= AT91C_RTC_SEC;
    // enable RXRDY interrupt in DBGU
    AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
