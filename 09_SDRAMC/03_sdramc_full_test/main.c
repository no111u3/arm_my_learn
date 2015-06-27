/**
 * Main code
 */
#include <AT91RM9200.h>
#include <util/util_int_str.c>
#include <util/util_screen.c>
// Memory size in MB
#define MEM_SIZE_M 128
// Memory size in kB
#define MEM_SIZE_K MEM_SIZE_M << 10
// Memory size in B
#define MEM_SIZE_B MEM_SIZE_M << 20
// Memory begin
#define MEM_BEGIN 0x00000000
// Memory end
#define MEM_END MEM_BEGIN + (MEM_SIZE_M << 20) - 1
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
    volatile unsigned int i, percent_counter = 0, errors = 0;
    volatile unsigned char mem_r, mem_w, counter = 0;
    char buffer[9];
    // base address of sdram region
    unsigned char * pt_mem_area = (unsigned char *)0x20000000;
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // begin-end write-read test
    put_string("Begin-end write-read test\n");
    put_string("Write test data\n");
    *(pt_mem_area + MEM_BEGIN) = 0x12;
    *(pt_mem_area + MEM_END) = 0x21;
    put_string("Read test data\n");
    mem_r = *(pt_mem_area + MEM_BEGIN);
    if (mem_r != 0x12) {
        put_string("Error: corrupt data in begin memory");
        put_string(" read: ");
        util_int_to_hex(mem_r, buffer);
        put_string(buffer);
        put_string("\n");
        errors++;
    }
    mem_r = *(pt_mem_area + MEM_END);
    if (mem_r != 0x21) {
        put_string("Error: corrupt data in end memory");
        put_string(" read: ");
        util_int_to_hex(mem_r, buffer);
        put_string(buffer);
        put_string("\n");
        errors++;
    }
    // 1MB point write-read test
    counter = 0;
    put_string("1MB point write-read test\n");
    put_string("Write test data\n");
    for (i = 0; i <= MEM_SIZE_M; i++) {
        mem_w = counter++;
        *(pt_mem_area + (i << 20)) = mem_w;
    }
    counter = 0;
    put_string("Read test data\n");
    for (i = 0; i <= MEM_SIZE_M; i++) {
        mem_r = *(pt_mem_area + (i << 20));
        mem_w = counter++;
        if (mem_r != mem_w) {
            put_string("Error: corrupt data in address-0x");
            util_int_to_hex(i << 20, buffer);
            put_string(buffer);
            put_string(" write: ");
            util_int_to_hex(mem_w, buffer);
            put_string(buffer);
            put_string(" read: ");
            util_int_to_hex(mem_r, buffer);
            put_string(buffer);
            put_string("\n");
            errors++;
        }
    }
    counter = 0;
    // 1kB write-read test
    put_string("1kB point write-read test\n");
    put_string("Write test data\n");
    for (i = 0; i <= MEM_SIZE_K; i++) {
        mem_w = counter++;
        *(pt_mem_area + (i << 10)) = mem_w;
    }
    counter = 0;
    put_string("Read test data\n");
    for (i = 0; i <= MEM_SIZE_K; i++) {
        mem_r = *(pt_mem_area + (i << 10));
        mem_w = counter++;
        if (mem_r != mem_w) {
            put_string("Error: corrupt data in address-0x");
            util_int_to_hex(i << 10, buffer);
            put_string(buffer);
            put_string(" write: ");
            util_int_to_hex(mem_w, buffer);
            put_string(buffer);
            put_string(" read: ");
            util_int_to_hex(mem_r, buffer);
            put_string(buffer);
            put_string("\n");
            errors++;
        }
    }
    counter = 0;
    // 1B write-read test
    put_string("1B point write-read test\n");
    put_string("Write test data\n");
    for (i = 0; i <= MEM_SIZE_B; i++) {
        mem_w = counter++;
        *(pt_mem_area + i) = mem_w;
    }
    counter = 0;
    put_string("Read test data\n");
    for (i = 0; i <= MEM_SIZE_B; i++) {
        mem_r = *(pt_mem_area + i);
        mem_w = counter++;
        if (mem_r != mem_w) {
            put_string("Error: corrupt data in address-0x");
            util_int_to_hex(i, buffer);
            put_string(buffer);
            put_string(" write: ");
            util_int_to_hex(mem_w, buffer);
            put_string(buffer);
            put_string(" read: ");
            util_int_to_hex(mem_r, buffer);
            put_string(buffer);
            put_string("\n");
            errors++;
        }
    }
    // printing results of tests
    if (errors) {
        put_string("Tests failed\n");
        util_int_to_hex(errors, buffer);
        put_string("Errors: ");
        put_string(buffer);
        put_string("\n");
    } else {
        put_string("Tests successed\n");
    }
    // Infinity loop
    while (1) {
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
