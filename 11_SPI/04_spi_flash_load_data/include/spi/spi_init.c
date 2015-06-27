/**
 * SPI init
 */
// SPI enable i/o pins
static void spi_pio_enable(void) {
    // enable pio
    AT91C_BASE_PIOA->PIO_ASR = AT91C_PA3_NPCS0 | AT91C_PA4_NPCS1 | AT91C_PA1_MOSI | AT91C_PA5_NPCS2 |
        AT91C_PA6_NPCS3 | AT91C_PA0_MISO | AT91C_PA2_SPCK;
    AT91C_BASE_PIOA->PIO_PDR = AT91C_PA3_NPCS0 | AT91C_PA4_NPCS1 | AT91C_PA1_MOSI | AT91C_PA5_NPCS2 |
        AT91C_PA6_NPCS3 | AT91C_PA0_MISO | AT91C_PA2_SPCK;
}
// SPI setup mode
static void spi_set_mode(const unsigned char mode) {
    // disable interrupts
    AT91C_BASE_SPI->SPI_IDR = (unsigned)-1;
    // reset spi_init
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SWRST;
    // setup mode
    if (mode == 'm') {
        // configure spi in master mode with no cs selected
        AT91C_BASE_SPI->SPI_MR = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS | AT91C_SPI_PCS;
        // enable spi
        AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SPIEN;
    }
}
// SPI init
static void spi_init(const unsigned char mode) {
    // enable spi pio
    spi_pio_enable();
    // configure pmc to enabling spi clock
    pmc_periph_clock_enable(AT91C_ID_SPI);
    // configure spi in current mode
    spi_set_mode(mode);
}
