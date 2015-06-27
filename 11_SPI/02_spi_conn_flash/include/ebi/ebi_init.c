/**
 * EBI init
 */
// EBI init
void ebi_init(void) {
    // setup MEMC to support all connected memories (CS1 = SDRAM)
    AT91C_BASE_EBI->EBI_CSA |= AT91C_EBI_CS1A_SDRAMC;
    AT91C_BASE_EBI->EBI_CFGR = 0;
}
