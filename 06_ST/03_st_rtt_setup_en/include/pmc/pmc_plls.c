/**
 * PMC PLL setup
 */

// PLLA setup
int plla_setup(const char plla_div, const char plla_count,
    const char plla_out, const char plla_mul) {
    return (((plla_div & 0xff) <<  0) | ((plla_count & 0x3f) <<  8) |
	((plla_out & 0x03) << 14) | ((plla_mul & 0x7ff) << 16) | 0x20000000);
}
// PLLB setup
int pllb_setup(const char pllb_div, const char pllb_count,
    const char pllb_out, const char pllb_mul, const char pllb_usb_96m) {
    return (((pllb_div & 0xff) <<  0) | ((pllb_count & 0x3f) <<  8) |
	((pllb_out & 0x03) << 14) | ((pllb_mul & 0x7ff) << 16) |
	((pllb_usb_96m & 0x01) << 28));
}
