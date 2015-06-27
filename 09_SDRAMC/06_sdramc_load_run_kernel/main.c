/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_screen.c>
#include <util/util_printf.c>
#include <dbgu/dbgu_xmod.c>
// SDRAM base address
#define SDRAM_BASE_ADDRESS  0x20000000
// SDRAM size in bytes
#define SDRAM_SIZE      (128 << 20)
// offset to linux kernel in sdram
#define LINUX_BASE_ADDRESS (SDRAM_BASE_ADDRESS + 0x00008000)
// offset to linux kernel tags in sdram
#define LINUX_TAGS_ADDRESS (SDRAM_BASE_ADDRESS + 0x00000100)
// offset to initrd in sdram
#define INITRD_ADDRESS (SDRAM_BASE_ADDRESS + 0x04000000)
// machine ident
#define MACHINE_ID 262
// global vars
volatile int temp;
// base address of sdram region
void * pt_mem_area;
// prototypes
extern void aic_asm_irq3_handler(void);
extern void aic_asm_sys_handler(void);
extern void aic_asm_tc_handler(void);
typedef void (*kernel_t) (unsigned, unsigned, void*);
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
    
    if (dbgu_status & AT91C_US_RXRDY) {
        // disable RXRDY interrupt in DBGU
        AT91C_BASE_DBGU->DBGU_IDR |= AT91C_US_RXRDY;
        // disable rtt interrupt flag
        AT91C_BASE_ST->ST_IDR = AT91C_ST_RTTINC;
        temp = dbgu_xmod_recv(pt_mem_area);
    }
    
    // handler of rtt - rttinc
    if (st_status & AT91C_ST_RTTINC) {
        AT91C_BASE_PIOB->PIO_ODSR ^= AT91C_PIO_PB27;
        put_char('C');
    }
}
// timer counter #0 handler
extern void tc_handler(void) {
    int status = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
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
// reset
static void reset(void) {
    put_string("\nQuit & Reset\n");
    AT91C_BASE_ST->ST_WDMR = 256 | AT91C_ST_RSTEN;
    AT91C_BASE_ST->ST_CR = AT91C_ST_WDRST;
}
// upload
int upload(void * address) {
    pt_mem_area = address;
    temp = 0;
    // setup rtt interrupt flag
    AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
    // enable RXRDY interrupt in DBGU
    AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
    while(!temp);
    delay(100000);
    if (temp > 0) {
        put_string("\n\nTransfer complete\n");
        util_printf("Byte's sended: 0x%x\n", temp);
    }
    else {
        put_string("Transfer failed\n!");
        temp = 0;
    }
    
    return temp;
}
// main function
extern void main(void) {
    kernel_t kernel = (kernel_t) LINUX_BASE_ADDRESS;
    unsigned *tags = (unsigned *) LINUX_TAGS_ADDRESS;
    unsigned *sz, len;
    const char *cmdline = "mem=128M root=/dev/ram rw initrd=0x24000000,0x500000 init=/sbin/init ramdisk_size=5000";
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    // uploading kernel
    put_string("Uploading kernel\n");
    if (upload((void *)LINUX_BASE_ADDRESS)) {
        put_string("Uploading kernel finaly\nUploading initrd\n");
	if (upload((void *)INITRD_ADDRESS)) {
            put_string("Uploading initrd finaly\n");
	}
	else {
            put_string("Upload failed\n");
            reset();
	}
    }
    else {
        put_string("Upload failed\n");
        reset();
    }
    if (temp > 0) {
        put_string("Upload success\nConfiguring\n");
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

        kernel(0, MACHINE_ID, (void *) LINUX_TAGS_ADDRESS);
    } else {
        put_string("Transfer failed\n");
    }
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
