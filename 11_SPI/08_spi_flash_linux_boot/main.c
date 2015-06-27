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
// SDRAM base address
#define SDRAM_BASE_ADDRESS  0x20000000
// SDRAM size in bytes
#define SDRAM_SIZE      (128 << 20)
// SPI AT45DB642D size
#define AT45DB642D_SIZE (8 << 20)
// offset to boot loader
#define BOOT_OFFSET 0x0
// offset to 2nd loader args
#define BOOT_2_ARGS_OFFSET AT45_PAGE_SIZE * 16
// offset to linux kernel in flash
#define LINUX_OFFSET AT45_PAGE_SIZE * 17
// offset to linux kernel in sdram
#define LINUX_BASE_ADDRESS (SDRAM_BASE_ADDRESS + 0x00008000)
// offset to linux kernel tags in sdram
#define LINUX_TAGS_ADDRESS (SDRAM_BASE_ADDRESS + 0x00000100)
// machine ident
#define MACHINE_ID 0
// global vars
volatile int transfer_size, wait_status;
// base address of sdram region
void * pt_mem_area = (unsigned int *) 0x24000000;
// base locations of flash regions
unsigned *upoint_r, *upoint_w;
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
extern void Jump(unsigned int addr);
static void run_kernel(void);
typedef void (*kernel_t) (unsigned, unsigned, void*);
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
        else {
            transfer_size = dbgu_xmod_recv((void *)LINUX_BASE_ADDRESS);
            run_kernel();
        }
    }
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        if (wait_status == 1) {
            put_char(counter);
            counter++;
            if (counter == '6')
                run_kernel();
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
// cmd args copy
static unsigned cmdcpy(unsigned *dst, const unsigned char *src) {
	unsigned char *dstc =(unsigned char *)dst;
	unsigned len = 0;

	while (*src) {
		*dstc++ = *src++;
		++len;
	}

	while (len & 3) {
		*dstc++ = ' ';
		++len;
	}

	return (len >> 2);
}
// run another code
static void run_kernel(void) {
    unsigned read_size;
    unsigned boot_args[AT45_PAGE_SIZE / 4];
    kernel_t kernel = (kernel_t) LINUX_BASE_ADDRESS;
    unsigned *tags = (unsigned *) LINUX_TAGS_ADDRESS;
    unsigned *sz, len;
    const char *cmdline = "mem=128M root=/dev/sda1 rootwait ro";
    
    /*if (!at45_read(BOOT_2_ARGS_OFFSET, boot_args, AT45_PAGE_SIZE)) {
        read_size = boot_args[0];
        util_printf("Read 0x%x bytes\n", read_size);
        if (read_size > 0 && read_size < (AT45DB642D_SIZE - 4)) {
            if (!at45_read(LINUX_OFFSET, (unsigned *)LINUX_BASE_ADDRESS, read_size)) {*/
                put_string("Read success\nConfiguring\n");
                /* atag_core */
                *tags++ = 5;
                *tags++ = 0x54410001;
                *tags++ = 1;
                *tags++ = 4096;
                *tags++ = 0;

                /* atag_mem */
                *tags++ = 4;
                *tags++ = 0x54410002;
                *tags++ = SDRAM_BASE_ADDRESS;
                *tags++ = SDRAM_SIZE;
                
                /* atag_cmdline */
                sz = tags++;
                *tags++ = 0x54410009;
                len = cmdcpy(tags, cmdline);
                *sz = 2 + len;
                tags += len;

                /* atag_none */
                *tags++ = 0;
                *tags++ = 0;
                
                util_printf("Jump to the kernel image\n\n");

                kernel(0, MACHINE_ID, (void*) LINUX_TAGS_ADDRESS);
            /*}
        }
    }*/
}
// main function
extern void main(void) {
    unsigned char flag, numb_pages;
    unsigned i, write_size, read_size,
        temp_six_vector, six_vector, errors, write_offset;
    unsigned boot_args[AT45_PAGE_SIZE / 4];
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
    
    upoint_r = pt_mem_area, upoint_w = (pt_mem_area + AT45DB642D_SIZE);
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
        put_string("\nload (l), write(w), run kernel(r), quit(q), erase(e): ");
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
                    put_string("Write boot(b) or linux kernel(n): ");
                    flag = get_char();
                    util_printf("%c\n", flag);
                    if (flag == 'b') {
                        write_offset = BOOT_OFFSET;
                        put_string("\nModification of Arm Interrupt Vector #6\n");
                        six_vector = (write_size / 512) + 1 + temp_six_vector;
                        util_printf("Six vector is 0x%x\n", six_vector);
                        upoint_w[5] = six_vector;
                    }
                    else {
                        write_offset = LINUX_OFFSET;
                        put_string("Writing args\n");
                        
                        boot_args[0] = write_size;
                        if (!at45_write(BOOT_2_ARGS_OFFSET, boot_args, AT45_PAGE_SIZE))
                            put_string("Write success\n");
                        else
                            put_string("Error!\n");
                    }
                    util_printf("Write 0x%x bytes\n", write_size);
                    
                    if (!at45_write(write_offset, upoint_w, write_size))
                        put_string("Write success\n");
                    else
                        put_string("Error!\n");
                    if (!at45_read(write_offset, upoint_r, write_size)) {
                        put_string("Read success\nStart verification\n");
                        six_vector = upoint_r[5];
                        if (write_offset == BOOT_OFFSET) {
                            if ((six_vector & 0xfffff000) - temp_six_vector) {
                                util_printf("Six vector is damage, current 0x%x, original 0x%x\n",
                                            six_vector, temp_six_vector);
                                break;
                            }
                            else {
                                put_string("Six vector is correct\nStart code verification\n");
                            }
                        }
                        errors = 0;
                        for (i = 0; i < write_size / 4; i++) {
                            if (upoint_r[i] != upoint_w[i]) {
                                errors++;
                                util_printf("Addr - %x, write - %x, read - %x\n", i, upoint_w[i], upoint_r[i]);
                            }
                        }
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
                run_kernel();
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
