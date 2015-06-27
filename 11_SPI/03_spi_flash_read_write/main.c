/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_mem.c>
#include <util/util_int_str.c>
#include <util/util_printf.c>
#include <spi/spi_re_tr.c>
#include <at45/at45.c>
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
// SPI AT45DB642D size
#define AT45DB642D_SIZE (8 << 20)
// data begin
#define DATA_B AT45_PAGE_SIZE * 43
// data size
#define DATA_S (AT45_PAGE_SIZE * 160)
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
// array view
static void array_view(unsigned * point, unsigned len) {
    unsigned i;
    for (i = 0; i < (len / 4); i++) {
        if (i >= 8 &&i % (unsigned)8 == 0)
            put_string(" ");
        if (i >= 16 && i % (unsigned)16 == 0)
            util_printf("0x%x\n", (point + i));
        util_printf("%x ", point[i]);
    }
    put_string("\n");
}
// main function
extern void main(void) {
    unsigned char flag;
    unsigned i, *upoint_r = pt_mem_area, *upoint_w = (pt_mem_area + AT45DB642D_SIZE * 2);
    put_string("Init AT45DB642D and get device information\n");
    if (!at45_init())
        put_string("Device inited and ready\n");
    else
        put_string("Error!\n");
    put_string("Test Array\n");
    for (i = 0; i < (DATA_S / 4); i++)
        upoint_w[i] = i;
    
    while (flag != 'q') {
        put_string("\n");
        put_string("one page test(1),\n");
        put_string("two page test(2),\n");
        put_string("four page test(3),\n");
        put_string("16 page test(4),\n");
        put_string("160 page test(5),\n");
        put_string("quit(q): ");
        flag = get_char();
        put_string("\n");
        switch(flag) {
            // one page test
            case '1':
                array_view(upoint_w, AT45_PAGE_SIZE);
                put_string("Read page\n");
                at45_page_read(DATA_B, upoint_r);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE);
                put_string("Page write\n");
                at45_wait_rdy();
                at45_buffer_fill(2, upoint_w);
                at45_wait_rdy();
                at45_page_write(2, DATA_B);
                at45_wait_rdy();
                put_string("Read page\n");
                at45_page_read(DATA_B, upoint_r);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE);
                put_string("Page erase\n");
                at45_page_erase(DATA_B);
            break;
            // two page test
            case '2':
                array_view(upoint_w, AT45_PAGE_SIZE * 2);
                put_string("Read page\n");
                at45_read_cont(DATA_B, upoint_r, AT45_PAGE_SIZE * 2);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 2);
                put_string("Page write\n");
                at45_write(DATA_B, upoint_w, AT45_PAGE_SIZE * 2);
                put_string("Read page\n");
                at45_read_cont(DATA_B, upoint_r, AT45_PAGE_SIZE * 2);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 2);
                put_string("Page erase\n");
                at45_page_erase(DATA_B);
                at45_page_erase(DATA_B + AT45_PAGE_SIZE);
            break;
            // four page test
            case '3':
                array_view(upoint_w, AT45_PAGE_SIZE * 4);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, AT45_PAGE_SIZE * 4);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 4);
                put_string("Page write\n");
                at45_write(DATA_B, upoint_w, AT45_PAGE_SIZE * 4);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, AT45_PAGE_SIZE * 4);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 4);
                put_string("Block erase\n");
                at45_block_erase(DATA_B);
            break;
            // 16 page test
            case '4':
                array_view(upoint_w, AT45_PAGE_SIZE * 16);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, AT45_PAGE_SIZE * 16);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 16);
                put_string("Page write\n");
                at45_write(DATA_B, upoint_w, AT45_PAGE_SIZE * 16);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, AT45_PAGE_SIZE * 16);
                put_string("Print page\n");
                array_view(upoint_r, AT45_PAGE_SIZE * 16);
                put_string("Block erase\n");
                at45_block_erase(DATA_B);
                at45_block_erase(DATA_B + AT45_BLOCK_SIZE);
            break;
            // 160 page test
            case '5':
                array_view(upoint_w, DATA_S);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, DATA_S);
                put_string("Print page\n");
                array_view(upoint_r, DATA_S);
                put_string("Page write\n");
                at45_write(DATA_B, upoint_w, DATA_S);
                put_string("Read page\n");
                at45_read(DATA_B, upoint_r, DATA_S);
                put_string("Print page\n");
                array_view(upoint_r, DATA_S);
                put_string("Block erase\n");
                at45_block_erase(DATA_B);
                at45_block_erase(DATA_B + AT45_BLOCK_SIZE);
            break;
            // exit of loop
            case 'q':
                put_string("\nQuit & Reset\n");
                AT91C_BASE_ST->ST_WDMR = 256 | AT91C_ST_RSTEN;
                AT91C_BASE_ST->ST_CR = AT91C_ST_WDRST;
            break;
            // undef
            default:
                put_string("\nUndefined command\n");
            break;
        }
    }
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
