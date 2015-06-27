/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
// service functions
void clear_screen(void) {
    int i;
    for (i = 0; i <= 20; i++)
        put_string("\b");
    for (i = 0; i <= 20; i++)
        put_string(" ");
    for (i = 0; i <= 20; i++)
        put_string("\b");
}
// update date
void update_date(void) {
    char buffer[9];
    int temp, temp_date;
    int new_date, old_date;
    clear_screen();
    // update year
    put_string("set year: ");
    get_string(buffer, 4, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x1900 && temp <= 0x2099) {
        temp_date |= (temp >> 8) & AT91C_RTC_CENT;
        temp_date |= (temp << 8) & AT91C_RTC_YEAR;
    }
    else {
         temp_date |= 0x0019;
    }
    clear_screen();
    // update month
    put_string("set month: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x1 && temp <= 0x12) {
        temp_date |= (temp << 16) & AT91C_RTC_MONTH;
    }
    else {
         temp_date |= 0x010000;
    }
    clear_screen();
    // update date
    put_string("set date: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x1 && temp <= 0x31) {
        temp_date |= (temp << 24) & AT91C_RTC_DATE;
    }
    else {
         temp_date |= 0x01000000;
    }
    clear_screen();
    old_date = AT91C_BASE_RTC->RTC_CALR;
    new_date = old_date & (AT91C_RTC_DAY);
    new_date |= temp_date;
    AT91C_BASE_RTC->RTC_CR |= AT91C_RTC_UPDCAL;
    while (~AT91C_BASE_RTC->RTC_SR & AT91C_RTC_ACKUPD);
    AT91C_BASE_RTC->RTC_CALR = new_date;
    if (AT91C_BASE_RTC->RTC_VER & AT91C_RTC_NVCAL) {
        put_string("invalid calendar\n");
        while (1);
    }
    AT91C_BASE_RTC->RTC_CR ^= AT91C_RTC_UPDCAL;
}
// update time
void update_time(void) {
    char buffer[9];
    int temp, temp_time;
    int new_time, old_time;
    clear_screen();
    // update hours
    put_string("set hour: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x23) {
        temp_time |= (temp << 16) & AT91C_RTC_HOUR;
    }
    clear_screen();
    // update minutes
    put_string("set min: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x59) {
        temp_time |= (temp << 8) & AT91C_RTC_MIN;
    }
    clear_screen();
    // update seconds
    put_string("set sec: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x59) {
        temp_time |= temp & AT91C_RTC_SEC;
    }
    clear_screen();
    old_time = AT91C_BASE_RTC->RTC_TIMR;
    if (!temp_time)
        new_time = old_time;
    else
        new_time = temp_time;
    util_int_to_hex(new_time, buffer);
    AT91C_BASE_RTC->RTC_CR |= AT91C_RTC_UPDTIM;
    while (~AT91C_BASE_RTC->RTC_SR & AT91C_RTC_ACKUPD);
    AT91C_BASE_RTC->RTC_TIMR = new_time;
    if (AT91C_BASE_RTC->RTC_VER & AT91C_RTC_NVTIM) {
        put_string("invalid time\n");
        while (1);
    }
    AT91C_BASE_RTC->RTC_CR ^= AT91C_RTC_UPDTIM;
}
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
        
        clear_screen();
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
    
    if (dbgu_status & AT91C_US_RXRDY) {
        char flag;
        // disable SEC interrupt in RTC
        AT91C_BASE_RTC->RTC_IDR |= AT91C_RTC_SEC;
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        flag = get_char();
        if (flag == 's') {
            clear_screen();
            put_string("set date(d) time(t)");
            flag = get_char();
            clear_screen();
            if (flag == 'd')
                update_date();
            else if (flag == 't')
                update_time();
        }
        // enable SEC interrupt in RTC
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
