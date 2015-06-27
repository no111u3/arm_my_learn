/**
* DBGU receive and transmit
*/
// putting char
void put_char(int character) {
    AT91C_BASE_DBGU->DBGU_THR = (character & 0xff);
}
// getting char
int get_char(void) {
    return ((AT91C_BASE_DBGU->DBGU_RHR) & 0xff);
}