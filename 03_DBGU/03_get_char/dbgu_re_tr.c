/**
* DBGU receive and transmit
*/
// putting char
void put_char(int character) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
    AT91C_BASE_DBGU->DBGU_THR = (character & 0xff);
}
// getting char
int get_char(void) {
    while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY));
    return ((AT91C_BASE_DBGU->DBGU_RHR) & 0xff);
}