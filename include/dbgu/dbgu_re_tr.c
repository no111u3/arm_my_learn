/**
* DBGU receive and transmit
*/
// putting char
extern void put_char(unsigned character) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
    AT91C_BASE_DBGU->DBGU_THR = (character & 0xff);
}
// getting char
extern unsigned get_char(void) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY));
    return ((AT91C_BASE_DBGU->DBGU_RHR) & 0xff);
}
// putting string
extern void put_string(const char * buffer) {
    while (*buffer) {
        put_char(*buffer++);
    }
}
// getting string
extern void get_string(char * buffer,unsigned int size, unsigned int in_out) {
    unsigned int end = (unsigned int)buffer + size;
    unsigned int temp;
    while ((unsigned int)buffer < end) {
        temp = get_char();
        if (in_out) {
            put_char(temp);
        }
        *buffer++ = temp;
    }
    *buffer = '\0';
}