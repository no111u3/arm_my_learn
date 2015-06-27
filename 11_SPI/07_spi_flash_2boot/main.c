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
// memory begin address
#define MEM_BEGIN 0x20000000
// SPI AT45DB642D size
#define AT45DB642D_SIZE (8 << 20)
// read address
#define READ_ADDRESS MEM_BEGIN
// read point
#define READ_P (void *) READ_ADDRESS
// write address
#define WRITE_ADDRESS MEM_BEGIN + AT45DB642D_SIZE
// write point
#define WRITE_P (void *) WRITE_ADDRESS
// offset to boot loader
#define BOOT_OFFSET 0x0
// offset to 2nd loader
#define BOOT_2_OFFSET 0x8400
// uboot ram address
#define UBOOT_ADDRESS MEM_BEGIN + 0x100000
// uboot size
#define UBOOT_SIZE 0x2ff40
// global vars
volatile int transfer_size, wait_status;
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
extern void Jump(unsigned int addr);
static void run_code(void);
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
    static unsigned char counter = '0';
    
    if (dbgu_status & AT91C_US_RXRDY) {
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        // disable rtt interrupt flag
        AT91C_BASE_ST->ST_IDR = AT91C_ST_RTTINC;
        if (wait_status == 1) {
            wait_status = 0;
        }
        else
            transfer_size = dbgu_xmod_recv(WRITE_P);
    }
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        if (wait_status == 1) {
            put_char(counter);
            counter++;
            if (counter == '6')
                run_code();
            put_char(' ');
        }
        else
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
// arrays verification
static unsigned arrays_verif(void *src, void *dest, unsigned len) {
    unsigned *usrc = src, *udest = dest, errors, i;
    errors = 0;
    for (i = 0; i < len / 4; i++) {
        if (usrc[i] != udest[i]) {
            errors++;
            util_printf("Addr - %x, src - %x, dest - %x\n", i * 4, usrc[i], udest[i]);
            if (errors > 100) {
                put_string("Over 100 errors\n");
                break;
            }
        }
    }
    
    return errors;
}
// run another code
static void run_code(void) {
    unsigned six_vector, read_size;
    read_size = UBOOT_SIZE;
    util_printf("\nRead 0x%x bytes\n", read_size);
    if (read_size > 0 && read_size < (AT45DB642D_SIZE - 4)) {
        if (!at45_read(BOOT_2_OFFSET, (void *)UBOOT_ADDRESS, read_size)) {
            put_string("Read success\n");
            put_string("Jump to loaded code\n");
            Jump(UBOOT_ADDRESS);
        }
    }
}
// main function
extern void main(void) {
    unsigned char flag, numb_pages;
    unsigned i, write_size, read_size,
        temp_six_vector, six_vector, errors, write_offset;
    unsigned *upoint_r = READ_P, *upoint_w = WRITE_P;
    // calculate temp six vector
    numb_pages = 0;
    i = AT45_PAGE_NUMB;
    while(i >>= 1)
        numb_pages++;
    temp_six_vector = (numb_pages << 13)  + (AT45_PAGE_SIZE << 17);
    
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    
    put_string("Init AT45DB642D and get device information\n");
    
    if (!at45_init())
        put_string("Device inited and ready\n");
    else
        put_string("Error!\n");
    
    put_string("Press any key to load boot menu\n: ");
    // setup rtt interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
    // enable RXRDY interrupt in DBGU
    AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
    wait_status = 1;
    get_char();

    while (flag != 'q') {
        put_string("\nload (l), write(w), run second code(r), quit(q), erase(e): ");
        flag = get_char();
        switch(flag) {
            // loading data to sdram
            case 'l':
                put_string("Please trasfer the boot file:\n");
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
            // writing bytes from data flash 
            case 'w':
                if (transfer_size == 0) {
                    put_string("Please transfer begin, write end\n");
                    break;
                }
                else if (transfer_size > (AT45DB642D_SIZE)) {
                    put_string("Trasfer is larger than flash size\n");
                    break;
                }
                else {
                    if ((unsigned)transfer_size % AT45_PAGE_SIZE)
                        write_size = ((unsigned)transfer_size / AT45_PAGE_SIZE + 1) * AT45_PAGE_SIZE;
                    else
                        write_size = transfer_size;
                        
                    put_string("Write boot(b) or boot2(n): ");
                    flag = get_char();
                    util_printf("%c\n", flag);
                    if (flag == 'b') {
                        write_offset = BOOT_OFFSET;
                        put_string("\nModification of Arm Interrupt Vector #6\n");
                        six_vector = (write_size / 512) + 1 + temp_six_vector;
                        util_printf("Six vector is 0x%x\n", six_vector);
                        upoint_w[5] = six_vector;
                        util_printf("Write 0x%x bytes\n", write_size);
                    }
                    else {
                        write_offset = BOOT_2_OFFSET;
                    }
                    
                    if (!at45_write(write_offset, upoint_w, write_size))
                        put_string("Write success\n");
                    else
                        put_string("Error!\n");

                    if (!at45_read(write_offset, upoint_r, write_size)) {
                        put_string("Read success\nStart verification\n");
                        if (write_offset == BOOT_OFFSET) {
                            six_vector = upoint_r[5];
                            if ((six_vector & 0xfffff000) - temp_six_vector) {
                                util_printf("Six vector is damage, current 0x%x, original 0x%x\n",
                                            six_vector, temp_six_vector);
                                break;
                            }
                            else {
                                put_string("Six vector is correct\nStart code verification\n");
                            }
                        }
                        errors = arrays_verif(upoint_w, upoint_r, write_size);
                        put_string("Stop code verification\n");
                        if (errors)
                            put_string("Verification failed!\n");
                        else
                            put_string("Verification success!\n");
                    }
                }
            break;
            // erase first page
            case 'e':
                if (!at45_read(0x0, upoint_r, 0x20)) {
                    six_vector = upoint_r[5];
                    read_size = (six_vector & 0xff) * 512;
                    for (i = 0; i <= read_size; i += AT45_PAGE_SIZE) {
                        if (!at45_page_erase(i))
                            put_string("Erase success\n");
                        else
                            put_string("Error!\n");
                    }
                }
            break;
            // run 2boot code
            case 'r':
                run_code();
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
