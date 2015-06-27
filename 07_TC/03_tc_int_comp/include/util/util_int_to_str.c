/**
 * UTIL convert int to string
 */
// UTIL unsigned int to hex string
void util_int_to_hex(unsigned int number, char *buffer) {
    const char *digits = "0123456789abcdef";
    const unsigned int base = 16;
    char temp[8];
    int counter = 0;
    if (number == 0)
        temp[counter++] = digits[0];
    else
        while (number !=0) {
            temp[counter++] = digits[number % base];
            number /= base;
        }
    counter--;
    while (counter >= 0)
        *(buffer++) = temp[counter--];
    *buffer = '\0';
}