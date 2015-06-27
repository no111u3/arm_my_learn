/**
 * PMC peripheral
 */
// PMC peripheral enable clock
static void pmc_periph_clock_enable(unsigned periph_id) {
    AT91C_BASE_PMC->PMC_PCER = 1 << periph_id;
}
// PMC peripheral disable clock
static void pmc_periph_clock_disable(unsigned periph_id) {
    AT91C_BASE_PMC->PMC_PCDR = 1 << periph_id;
}
