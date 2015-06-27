/**
 * UTIL printf
 */
#include <stdarg.h>
// UTIL vfprintf
unsigned util_vprintf(const char * format, va_list va) {
    char * s, buffer[9];
    
    do {
        if (*format == '%') {
            format++;
            if (*format == 's') {
                s = va_arg(va, char *);
                if (s)
                    put_string(s);
                else
                    put_string("<NULL>");
            }
            else if (*format == 'c') {
                put_char(va_arg(va, unsigned));
            }
            else if (*format == 'x') {
                util_int_to_hex(va_arg(va, unsigned), buffer);
                put_string(buffer);
            }
            else {
                put_string("%");
            }
        }
        else {
            put_char(*format);
        }
        format++;
    } while (*format);
    
    return 0;
}
// UTIL printf
unsigned util_printf(const char * format, ...) {
    va_list ap;
    unsigned flag;

    va_start(ap, format);
    flag = util_vprintf(format, ap);
    va_end(ap);

    return flag;
}