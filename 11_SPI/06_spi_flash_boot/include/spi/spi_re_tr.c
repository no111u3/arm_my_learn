/**
 * SPI recieve transmit
 */
// SPI struct transfer block
struct spi_t_block {
    void *spi_rx;
    const void *spi_tx;
    unsigned spi_t_len;
};
// SPI tarnsfer block
static void spi_transfer(struct spi_t_block * t_block, int n) {
    AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTDIS | AT91C_PDC_RXTDIS;
    
    AT91C_BASE_SPI->SPI_TPR = (unsigned) t_block[0].spi_tx;
    AT91C_BASE_SPI->SPI_TCR = t_block[0].spi_t_len;
    AT91C_BASE_SPI->SPI_RPR = (unsigned) t_block[0].spi_rx;
    AT91C_BASE_SPI->SPI_RCR = t_block[0].spi_t_len;
    
    if (n > 1) {
        AT91C_BASE_SPI->SPI_TNPR = (unsigned) t_block[1].spi_tx;
        AT91C_BASE_SPI->SPI_TNCR = t_block[1].spi_t_len;
        AT91C_BASE_SPI->SPI_RNPR = (unsigned) t_block[1].spi_rx;
        AT91C_BASE_SPI->SPI_RNCR = t_block[1].spi_t_len;
    }
    
    AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTEN | AT91C_PDC_RXTEN;
    while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RXBUFF));

    AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_TXTDIS | AT91C_PDC_RXTDIS;
}