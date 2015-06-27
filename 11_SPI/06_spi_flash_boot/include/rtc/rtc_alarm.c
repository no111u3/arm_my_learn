/**
 * RTC alarm setup
 */
// RTC alarm setup time
static void rtc_alarm_time(void) {
    char flag;
    char buffer[9];
    int temp, temp_time;
    util_clear_screen();
    // setup alarm second
    put_string("en alrm sec(y/n): ");
    flag = get_char();
    if (flag == 'y') {
        temp_time |= AT91C_RTC_SECEN;
        util_clear_screen();
        // update seconds
        put_string("set sec: ");
        get_string(buffer, 2, 1);
        temp = util_hex_to_int(buffer);
        if (temp >= 0x00 && temp <= 0x59) {
            temp_time |= temp & AT91C_RTC_SEC;
        }
        util_clear_screen();
    }
    // setup alarm minute
    put_string("en alrm min(y/n): ");
    flag = get_char();
    if (flag == 'y') {
        temp_time |= AT91C_RTC_MINEN;
        util_clear_screen();
        // update minutes
        put_string("set min: ");
        get_string(buffer, 2, 1);
        temp = util_hex_to_int(buffer);
        if (temp >= 0x00 && temp <= 0x59) {
            temp_time |= (temp << 8) & AT91C_RTC_MIN;
        }
        util_clear_screen();
    }
    // setup alarm minute
    put_string("en alrm hour(y/n): ");
    flag = get_char();
    if (flag == 'y') {
        temp_time |= AT91C_RTC_HOUREN;
        util_clear_screen();
        // update hours
        put_string("set hour: ");
        get_string(buffer, 2, 1);
        temp = util_hex_to_int(buffer);
        if (temp >= 0x00 && temp <= 0x23) {
            temp_time |= (temp << 16) & AT91C_RTC_HOUR;
        }
        util_clear_screen();
    }
    if (temp_time & AT91C_RTC_SECEN) {
        AT91C_BASE_RTC->RTC_TIMALR = temp_time;
        AT91C_BASE_RTC->RTC_IER |= AT91C_RTC_ALARM;
    }
}
