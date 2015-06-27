/**
* DBGU receive and transmit
*/
// putting char
extern void put_char(int character) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
    AT91C_BASE_DBGU->DBGU_THR = (character & 0xff);
}
// getting char
int get_char(void) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY));
    return ((AT91C_BASE_DBGU->DBGU_RHR) & 0xff);
}
// putting string
extern void put_string(const char * buffer) {
    while (*buffer != '\0') {
        put_char(*buffer++);
    }
}
// getting string
void get_string(char * buffer, int size, int in_out) {
    int end = (int)buffer + size;
    int temp;
    while ((int)buffer < end) {
        temp = get_char();
        if (in_out) {
            put_char(temp);
        }
        *buffer++ = temp;
    }
    *buffer = '\0';
}