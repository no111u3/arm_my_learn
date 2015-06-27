/**
 * TWI init
 */
// TWI enable i/o pins
void twi_pio_enable(void) {
    // enable pio
    AT91C_BASE_PIOA->PIO_ASR |= AT91C_PA25_TWD | AT91C_PA26_TWCK; 
    AT91C_BASE_PIOA->PIO_PDR |= AT91C_PA25_TWD | AT91C_PA26_TWCK;
    // condigure pa25-twd to opendrain
    AT91C_BASE_PIOA->PIO_MDDR = ~AT91C_PA25_TWD;
    AT91C_BASE_PIOA->PIO_MDER = AT91C_PA25_TWD;
}
// TWI setup mode
void twi_set_mode(const unsigned char mode) {
    // disable interrupts
    AT91C_BASE_TWI->TWI_IDR = (unsigned)-1;
    // reset peripheral
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
    // setup mode
    if (mode == 'm')
        AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN | AT91C_TWI_SVDIS;
}
// TWI setup clock
void twi_set_clock(const unsigned master_clock, const unsigned twi_clock) {
    unsigned int sclock;
    // CKDIV = 1 and CHDIV == CLDIV ==> CLDIV = CHDIV = 1/4 * ((Fmclk/FTWI) - 6)
    sclock = (10 * master_clock / twi_clock);
    
    if (sclock % 10 >= 5)
        sclock = (sclock / 10) - 5;
    else
        sclock = (sclock / 10) - 6;
    sclock = (sclock + (4 - sclock % 4)) >> 2; // div on 4
    
    AT91C_BASE_TWI->TWI_CWGR = 0x00010000 | sclock | (sclock << 8);
}
// TWI init
void twi_init(const unsigned char mode, const unsigned master_clock, const unsigned twi_clock) {
    // enable twi pio
    twi_pio_enable();
    // configure pmc to enabling twi clock
    pmc_periph_clock_enable(AT91C_ID_TWI);
    // configure twi in current mode
    twi_set_mode(mode);
    // setup twi clock
    twi_set_clock(master_clock, twi_clock);
}
