/**
 * Main code
 */
#include <AT91RM9200.h>
#include <dbgu/dbgu_xmod.c>
#include <util/util_mem.c>
#include <util/util_int_str.c>
#include <util/util_printf.c>
#include <spi/spi_re_tr.c>
#include <at45/at45.c>
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
// global vars
volatile int transfer_size;
// base address of sdram region
void *pt_mem_area = (unsigned int *) 0x20000000;
// base locations of flash regions
unsigned *upoint_r, *upoint_w;
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
extern void Jump(unsigned int addr);
// external interrupt 1 handler
extern void eint3_handler(void) {
    aic_disable_irq(AT91C_ID_IRQ3);
    aic_enable_irq(AT91C_ID_IRQ3);
}
// system interrupt handler
extern void sys_handler(void) {
    unsigned st_status = AT91C_BASE_ST->ST_SR & AT91C_BASE_ST->ST_IMR;
    unsigned rtc_status = AT91C_BASE_RTC->RTC_SR & AT91C_BASE_RTC->RTC_IMR;
    unsigned dbgu_status = AT91C_BASE_DBGU->DBGU_CSR & AT91C_BASE_DBGU->DBGU_IMR;
    
    if (dbgu_status & AT91C_US_RXRDY) {
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        // disable rtt interrupt flag
        AT91C_BASE_ST->ST_IDR = AT91C_ST_RTTINC;
        transfer_size = dbgu_xmod_recv(upoint_w + 1);
    }
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        put_char('C');
    }
}
// timer counter #0 handler
extern void tc_handler(void) {
    unsigned status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
}
// array view
static void array_view(unsigned *point, unsigned len) {
    unsigned i;
    for (i = 0; i < (len / 4); i++) {
        if (i >= 4 &&i % (unsigned)4 == 0)
            put_string(" ");
        if (i >= 8 && i % (unsigned)8 == 0)
            util_printf("0x%x\n", (point + i));
        util_printf("%x ", point[i]);
    }
    put_string("\n");
}
// main function
extern void main(void) {
    unsigned char flag;
    unsigned i, write_size, read_size, errors;
    
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    
    upoint_r = pt_mem_area, upoint_w = (pt_mem_area + AT45DB642D_SIZE);
    put_string("Init AT45DB642D and get device information\n");
    
    if (!at45_init())
        put_string("Device inited and ready\n");
    else
        put_string("Error!\n");

    while (flag != 'q') {
        put_string("\nload(l), read(r), write(w), run(s), quit(q), erase(e): ");
        flag = get_char();
        put_string("\n");
        switch(flag) {
            // loading data to sdram
            case 'l':
                transfer_size = 0;
                // setup rtt interrupt flag
                AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
                // enable RXRDY interrupt in DBGU
                AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
                while(!transfer_size);
                delay(100000);
                if (transfer_size > 0) {
                    put_string("Transfer complete\n");
                    util_printf("Byte's sended: %x\n", transfer_size);
                }
            break;
            // reading bytes from data flash
            case 'r':
                if (!at45_read(0x0, &read_size, 0x4)) {
                    if (read_size > 0 && read_size < (AT45DB642D_SIZE - 4)) {
                        util_printf("Read 0x%x bytes\n", read_size);
                        if (!at45_read(0x4, upoint_r, read_size)) {
                            put_string("Read success\nPrint readed bytes\n");
                            array_view(upoint_r, read_size);
                        }
                    }
                    else {
                        put_string("Data is damage or empty\n");
                    }
                }
                else
                    put_string("Error!\n");
            break; 
            case 'w':
                if (transfer_size == 0) {
                    put_string("Please transfer begin, write end\n");
                    break;
                }
                else if (transfer_size > (AT45DB642D_SIZE - 4)) {
                    put_string("Trasfer is larger than flash size\n");
                    break;
                }
                else {
                    *upoint_w = transfer_size;
                    transfer_size += 4;
                    if ((unsigned)transfer_size % AT45_PAGE_SIZE)
                        write_size = (unsigned)transfer_size / AT45_PAGE_SIZE * AT45_PAGE_SIZE + AT45_PAGE_SIZE;
                    else
                        write_size = transfer_size;
                    util_printf("\nWrite 0x%x bytes\n", write_size);
                    array_view(upoint_w, write_size);
                    if (!at45_write(0x0, upoint_w, write_size))
                        put_string("Write success\n");
                    else
                        put_string("Error!\n");
                    put_string("Verification\n");
                    errors = 0;
                    if (!at45_read(0x0, upoint_r, write_size)) {
                        put_string("Read success\nStart verification\n");
                        for (i = 0; i < write_size / 4; i++) {
                            if (upoint_r[i] != upoint_w[i]) {
                                errors++;
                                util_printf("Addr - %x, write - %x, read - %x\n", i, upoint_w[i], upoint_r[i]);
                            }
                        }
                        if (errors)
                            put_string("Verification failed!\n");
                        else
                            put_string("Verification success!\n");
                    }
                }
            break;
            // run loaded code
            case 's':
                if (!at45_read(0x0, &read_size, 0x4)) {
                    util_printf("Read 0x%x bytes\n", read_size);
                    if (read_size > 0 && read_size < (AT45DB642D_SIZE - 4)) {
                        if (!at45_read(0x4, pt_mem_area, read_size)) {
                            put_string("Read success\n");
                            put_string("Jump to loaded code\n");
                            Jump(0x20000000);
                        }
                    }
                }
            // erase first page
            case 'e':
                if (!at45_page_erase(0x0))
                    put_string("Erase success\n");
                else
                    put_string("Error!\n");
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
