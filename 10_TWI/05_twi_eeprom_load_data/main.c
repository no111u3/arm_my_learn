/**
 * Main code
 */
#include <AT91RM9200.h>
#include <dbgu/dbgu_xmod.c>
#include <twi/twi_re_tr.c>
#include <util/util_int_str.c>
#include <util/util_printf.c>
// global vars
volatile int temp;
// base address of sdram region
void * pt_mem_area = (unsigned int *) 0x20000000;
void * pt_mem_area2 = (unsigned int *) 0x2000ffff;
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
// main function
extern void main(void) {
    char flag;
    unsigned short data_size;
    unsigned short counter;
    unsigned * data;
    volatile int i;
    // setup SYS interrupt
    aic_configure_irq(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST,
                      AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, aic_asm_sys_handler);
    // enable SYS interrupt
    aic_enable_irq(AT91C_ID_SYS);
    // setup rtt - 1Hz clock
    AT91C_BASE_ST->ST_RTMR = 0x4000;
    // Infinity loop
    while (1) {
        put_string("Load data(l), write eeprom(w), read eeprom(r)\n");
        flag = get_char();
        switch (flag) {
            case 'l':
                temp = 0;
                // setup rtt interrupt flag
                AT91C_BASE_ST->ST_IER = AT91C_ST_RTTINC;
                // enable RXRDY interrupt in DBGU
                AT91C_BASE_DBGU->DBGU_IER |= AT91C_US_RXRDY;
                while(!temp);
                delay(100000);
                if (temp > 0) {
                put_string("Transfer complete\n");
                util_printf("Byte's sended: %x\n", temp);
                }
                break;
            case 'w':
                put_string("Write data to eeprom\n");
                if (temp > 0xfffd) {
                    put_string("Data is biggest than eeprom\nWrited first 0xfffd bytes\n");
                    data_size = (unsigned short)0xfffd;
                }
                else {
                    util_printf("Data size is %x bytes\n", temp);
                    data_size = (unsigned short)temp;
                }
                util_printf("Write data, size: %x\n", data_size);
                twi_write(TWI_EEPROM_ADDRESS, 0x0, (char *)&data_size, 2);
                for (counter = 2; counter <= data_size + 2; counter += 128) {
                    twi_write(TWI_EEPROM_ADDRESS, counter, (char *)(pt_mem_area + counter), 128);
                    put_char('.');
                }
                // wait 10ms before data is written into EEPROM
                for (i = 0; i < 100000; i++);
                twi_read(TWI_EEPROM_ADDRESS, 0x0, (char *)&data_size, 2);
                util_printf("Data Writed: %x bytes\n", data_size);
                break;
            case 'r':
                // wait 10ms before data is written into EEPROM
                for (i = 0; i < 100000; i++);
                twi_read(TWI_EEPROM_ADDRESS, 0x0, (char *)&data_size, 2);
                util_printf("Read data, size: %x\n", data_size);
                twi_read(TWI_EEPROM_ADDRESS, 0x2, (char *)pt_mem_area2, data_size);
                put_string("Data Readed\nPrint data\n");
                for (counter = 0; counter <= data_size; counter += 4) {
                    data = (unsigned *)(pt_mem_area2 + counter);
                    util_printf("Address: 0x%x - Data: 0x%x\n",
                                (unsigned)(data), *data);
                }
                break;
        }
        // Disable pck for idle cpu mode
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }
}
