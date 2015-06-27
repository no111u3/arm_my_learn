/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_printf.c>
#include <spi/spi_re_tr.c>
// base address of sdram region
void * pt_mem_area = (unsigned int *) 0x20000000;
// Memory size in MB
#define MEM_SIZE_M 64
// Memory begin
#define MEM_BEGIN 0x00000000
// Memory end
#define MEM_END MEM_BEGIN + (MEM_SIZE_M << 20) - 1
// TWI rtc address
#define TWI_RTC_ADDRESS 0x68
#define TWI_EEPROM_ADDRESS 0x50
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
// external interrupt 1 handler
extern void eint3_handler(void) {
    aic_disable_irq(AT91C_ID_IRQ3);
    aic_enable_irq(AT91C_ID_IRQ3);
}
// system interrupt handler
extern void sys_handler(void) {
    int st_status = AT91C_BASE_ST->ST_SR & AT91C_BASE_ST->ST_IMR;
    int rtc_status = AT91C_BASE_RTC->RTC_SR & AT91C_BASE_RTC->RTC_IMR;
    int dbgu_status = AT91C_BASE_DBGU->DBGU_CSR & AT91C_BASE_DBGU->DBGU_IMR;
}
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
}
// main function
extern void main(void) {
    unsigned char temp[4];
    unsigned data_len = 4;
    struct spi_t_block t_block;
    
    put_string("Get device information\n");
    // Get device information
    temp[0] = 0x9f;
    
    t_block.spi_rx = temp;
    t_block.spi_tx = temp;
    t_block.spi_t_len = 4;
    // select at45db642d spi flash
    spi_select(0);
    // transfer data
    spi_transfer(t_block);
    
    util_printf("Manufacturer ID: %x\n", temp[1]);
    util_printf("Density Code: %x\n", temp[2]);
    util_printf("Product Version: %x\n", temp[3]);
    
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
