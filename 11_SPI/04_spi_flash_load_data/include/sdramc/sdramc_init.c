/** * SDRAMC initing */#define SDRAMC_PORTC_SEL_LINE \    AT91C_PC16_D16 | AT91C_PC17_D17 | AT91C_PC18_D18 | AT91C_PC19_D19 | \    AT91C_PC20_D20 | AT91C_PC21_D21 | AT91C_PC22_D22 | AT91C_PC23_D23 | \    AT91C_PC24_D24 | AT91C_PC25_D25 | AT91C_PC26_D26 | AT91C_PC27_D27 | \    AT91C_PC28_D28 | AT91C_PC29_D29 | AT91C_PC30_D30 | AT91C_PC31_D31 // Write Recovery Delay#define AT91Cmy_SDRAM_TWR 2// Row Cycle Delay#define AT91Cmy_SDRAM_TRC 2// Row Precharge Delay#define AT91Cmy_SDRAM_TRP 2// Row to Column Delay#define AT91Cmy_SDRAM_TRCD 2// Active to Precharge Delay#define AT91Cmy_SDRAM_TRAS 2// Exit Self Refresh to Active Delay#define AT91Cmy_SDRAM_TXSR 2// WB = 0, OpMode = 0, CAS = 2, BT = 0, Burst lenght = 0 (1)#define AT91Cmy_SDRAM_MR_CHIP 0x0080//old -> 0x2E0  // 0x170 -> 60 MHz; 0x293 -> 90Mhz; 0x61 -> 16MHz#define AT91Cmy_SDRAM_Refresh 0x61// SDRAMC enable i/o pinsstatic void sdramc_pio_enable(void) {    AT91C_BASE_PIOC->PIO_PDR = SDRAMC_PORTC_SEL_LINE;    AT91C_BASE_PIOC->PIO_ASR = SDRAMC_PORTC_SEL_LINE;    AT91C_BASE_PIOC->PIO_PPUER = SDRAMC_PORTC_SEL_LINE;    AT91C_BASE_PIOC->PIO_MDDR = SDRAMC_PORTC_SEL_LINE;        AT91C_BASE_PIOC->PIO_IFDR = SDRAMC_PORTC_SEL_LINE;    AT91C_BASE_PIOC->PIO_IDR = SDRAMC_PORTC_SEL_LINE;    AT91C_BASE_PIOC->PIO_OWDR = SDRAMC_PORTC_SEL_LINE;}// SDRAMC initstatic void sdramc_init(void) {    volatile int i;    // base address of sdram region    unsigned int * pt_mem_area = (unsigned int *) 0x20000000;    // enable SDRAMC pio    sdramc_pio_enable();    // configuring and initing SDRAM    AT91C_BASE_SDRC->SDRC_CR = AT91C_SDRC_NC_10 | // Column A0-A9        AT91C_SDRC_NR_13 | // Row    A0-A12        AT91C_SDRC_NB_4_BANKS | // numbers Bank 4        AT91C_SDRC_CAS_2 | // CAS Latency 2        ((AT91Cmy_SDRAM_TWR <<  7) & AT91C_SDRC_TWR) |        ((AT91Cmy_SDRAM_TRC << 11) & AT91C_SDRC_TRC) |        ((AT91Cmy_SDRAM_TRP << 15) & AT91C_SDRC_TRP) |        ((AT91Cmy_SDRAM_TRCD << 19) & AT91C_SDRC_TRCD) |        ((AT91Cmy_SDRAM_TRAS << 23) & AT91C_SDRC_TRAS) |        ((AT91Cmy_SDRAM_TXSR << 27) & AT91C_SDRC_TXSR);    // al all banks precharge command is issued to SDRAM devices    AT91C_BASE_SDRC->SDRC_MR = AT91C_SDRC_MODE_PRCGALL_CMD | AT91C_SDRC_DBW_32_BITS;    pt_mem_area[0] = 0x00000000;    // eight auot-refresh (CBR) cycles are provided    for (i = 0; i < 8; i++) {        AT91C_BASE_SDRC->SDRC_MR = AT91C_SDRC_MODE_RFSH_CMD | AT91C_SDRC_DBW_32_BITS;        *pt_mem_area = 0;    }    // a mode register set (MRS) cycle is issued to program the parameters of the SDRAM    // devices, in particular CAS latency and burst length    AT91C_BASE_SDRC->SDRC_MR = AT91C_SDRC_MODE_LMR_CMD | AT91C_SDRC_DBW_32_BITS;    *(pt_mem_area +AT91Cmy_SDRAM_MR_CHIP) = 0x00000000;    // write refresh rate into the count field in the SDRAMC refresh timer register. (Refresh    // rate = delay between refresh cycles)    AT91C_BASE_SDRC->SDRC_TR = AT91Cmy_SDRAM_Refresh & AT91C_SDRC_COUNT;    pt_mem_area[0] = 0x00000000;    // a normal mode command is provided, 3 clocks after tMRD is met    AT91C_BASE_SDRC->SDRC_MR = AT91C_SDRC_MODE_NORMAL_CMD  | AT91C_SDRC_DBW_32_BITS;    pt_mem_area[0] = 0x00000000;}