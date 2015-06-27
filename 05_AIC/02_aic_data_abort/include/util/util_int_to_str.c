/**
 * UTIL convert int to string
 */
// UTIL unsigned int to hex string
void util_int_to_hex(unsigned int number, char *buffer) {
    const char *digits = "0123456789abcdef";
    const unsigned int base = 16;
    if (number == 0)
        *buffer++ = digit[0];
    else
        while number !=0
            *buffer++ = digits[digit % base];
            digit /= base;
    *buffer = "\0";
}