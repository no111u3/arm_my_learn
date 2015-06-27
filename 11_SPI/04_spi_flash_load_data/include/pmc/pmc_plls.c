/**
 * PMC PLL setup
 */

// PLLA setup
static unsigned plla_setup(const unsigned char plla_div, const unsigned char plla_count,
    const unsigned char plla_out, const unsigned char plla_mul) {
    return (((plla_div & 0xff) <<  0) | ((plla_count & 0x3f) <<  8) |
	((plla_out & 0x03) << 14) | ((plla_mul & 0x7ff) << 16) | 0x20000000);
}
// PLLB setup
static unsigned pllb_setup(const unsigned char pllb_div, const unsigned char pllb_count,
    const unsigned char pllb_out, const unsigned char pllb_mul,
    const unsigned char pllb_usb_96m) {
    return (((pllb_div & 0xff) <<  0) | ((pllb_count & 0x3f) <<  8) |
	((pllb_out & 0x03) << 14) | ((pllb_mul & 0x7ff) << 16) |
	((pllb_usb_96m & 0x01) << 28));
}
