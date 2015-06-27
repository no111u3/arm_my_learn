/**
 * TWI recieve transmit
 */
// TWI read byte
unsigned char twi_read_byte(unsigned char device_id, unsigned char address) {
    unsigned int status;
    unsigned char data, temp_data;
    // set twi master mode register
    AT91C_BASE_TWI->TWI_MMR = (device_id << 16) | AT91C_TWI_IADRSZ_1_BYTE | AT91C_TWI_MREAD;
    // set twi internal address register
    AT91C_BASE_TWI->TWI_IADR = address;
    // start transfer
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
    
    status = AT91C_BASE_TWI->TWI_SR;
    // wait RHR holding register is full
    while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_RXRDY));
    // recieve byte
    data = (unsigned char)AT91C_BASE_TWI->TWI_RHR;
    
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
    
    status = AT91C_BASE_TWI->TWI_SR;
    // wait transfer is finished
    while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP));
    // recieve last unuse byte
    temp_data = (unsigned char)AT91C_BASE_TWI->TWI_RHR;
    
    return data;
}
// TWI write byte
void twi_write_byte(unsigned char device_id, unsigned char address, unsigned char data) {
    unsigned int status;
    // set twi master mode register
    AT91C_BASE_TWI->TWI_MMR = (device_id << 16) | AT91C_TWI_IADRSZ_1_BYTE & ~AT91C_TWI_MREAD;
    // set twi internal address register
    AT91C_BASE_TWI->TWI_IADR = address;
    // start transfer
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
    
    status = AT91C_BASE_TWI->TWI_SR;
    // transmit byte
    AT91C_BASE_TWI->TWI_THR = data;
    // wait THR holding register is full
    while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY));
    
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
    
    status = AT91C_BASE_TWI->TWI_SR;
    // wait transfer is finished
    while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP));
}
