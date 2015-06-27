/**
 * Main code
 */
#include <AT91RM9200.h>
#include <dbgu/dbgu_re_tr.c>

// main function
extern void main(void) {
    AT91C_BASE_PIOB->PIO_CODR = AT91C_PIO_PB27;
    
    put_string("Test message from loaded code\n");
    put_string("Code loaded from external spi flash\n");
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
