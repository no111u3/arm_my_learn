/**
 * UTIL printf
 */
#include <stdarg.h>
// UTIL printf
static unsigned util_printf(const char * format, ...) {
    char * s, buffer[9];
    va_list ap;
    
    va_start(ap, format);
    
    do {
        if (*format == '%') {
            format++;
            if (*format == 's') {
                s = va_arg(ap, char *);
                if (s)
                    put_string(s);
                else
                    put_string("<NULL>");
            }
            else if (*format == 'c') {
                put_char(va_arg(ap, unsigned));
            }
            else if (*format == 'x') {
                util_int_to_hex(va_arg(ap, unsigned), buffer);
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
    
    va_end(ap);
    
    return 0;
}