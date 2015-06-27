/**
 * RTC clock setup
 */
// RTC clock update date
void rtc_update_date(void) {
    char buffer[9];
    int temp, temp_date;
    int new_date, old_date;
    util_clear_screen();
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
    util_clear_screen();
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
    util_clear_screen();
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
    util_clear_screen();
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
// RTC clock update time
void rtc_update_time(void) {
    char buffer[9];
    int temp, temp_time;
    int new_time, old_time;
    util_clear_screen();
    // update hours
    put_string("set hour: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x23) {
        temp_time |= (temp << 16) & AT91C_RTC_HOUR;
    }
    util_clear_screen();
    // update minutes
    put_string("set min: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x59) {
        temp_time |= (temp << 8) & AT91C_RTC_MIN;
    }
    util_clear_screen();
    // update seconds
    put_string("set sec: ");
    get_string(buffer, 2, 1);
    temp = util_hex_to_int(buffer);
    if (temp >= 0x00 && temp <= 0x59) {
        temp_time |= temp & AT91C_RTC_SEC;
    }
    util_clear_screen();
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
