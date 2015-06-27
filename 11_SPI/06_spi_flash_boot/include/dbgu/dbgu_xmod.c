/**
 * DBGU x-modem
 */
#define XM_NAK      0x15
#define XM_SOH      0x01
#define XM_ACK      0x06
#define XM_EOT      0x04
// DBGU crc16
static unsigned short dbgu_crc16(unsigned char *ptr, int len) {
    unsigned short crc, i;
    crc = 0x0000;
    while (len--) {
        crc ^= (unsigned short) *ptr++ << 8;
        for (i = 0; i < 8; i++)
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
    }
    return crc;
}
// DBGU x-modem reciever
unsigned int dbgu_xmod_recv(void * ptr) {
    unsigned char *ptr_w = ptr;
    unsigned char *ptr_fix;
    unsigned char c, blk = 1;
    unsigned short crc, rcrc;
    unsigned int i, n = 0;
    int err, fail;
    
    for (;;) {
        err = 0;
        fail = 0;

        do {
            c = get_char();
            if (c != XM_SOH) {
                if (c == XM_EOT) {
                    put_char(XM_ACK);
                    AT91C_BASE_PIOB->PIO_ODSR |= AT91C_PIO_PB27;
                    return (128 * n);
                }

                fail = 1;
                break;
            }

            c = get_char();
            if (c != blk)
                fail = 1;
            c = get_char();
            if (c != (0xff - blk))
                fail = 1;

            ptr_fix = ptr_w;

            for (i = 0; i < 128; ++i)
                *ptr_w++ = get_char();

            c = get_char();
            rcrc = (unsigned short) c << 8;
            c = get_char();
            rcrc |= (unsigned short) c;

            crc = dbgu_crc16(ptr_fix, 128);

            if (crc != rcrc)
                fail = 1;

            if (fail)
                put_char(XM_NAK);
            else {
                put_char(XM_ACK);
                break;
            }

            err += fail;
            fail = 0;

            if (err > 4) {
                return -1;
            }

        } while (1);

        if (!fail) {
            ++blk;
            ++n;
        }
    }
}
