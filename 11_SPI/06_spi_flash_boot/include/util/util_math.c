/**
 * UTIL math
 */
// UTIL unsigned 32 divide-module
static void util_udivmod(unsigned num, unsigned den, unsigned *result, unsigned *rem) {
	unsigned ret = 0;
	unsigned sub_val, sub_val_s;
	unsigned bit, bit_s;
	unsigned done;

	if (den == 0)
		return;

	do {
		sub_val = sub_val_s = den;
		bit = bit_s = 0;
		done = 1;

		do {
			if (sub_val_s > num)
				break;
			else {
				sub_val = sub_val_s;
				bit = bit_s;
			}

			sub_val_s <<= 1;
			++bit_s;

			done = 0;

		} while (1);

		if (done)
			break;
		else {
			ret |= (1 << bit);
			num -= sub_val;
		}

	} while (1);

	if (result) {
		*result = ret;
	}

	if (rem) {
		*rem = num;
	}
}