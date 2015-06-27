/**
 * UTIL screen
 */
// clear screen
void util_clear_screen(void) {
    int i;
    for (i = 0; i <= 20; i++)
        put_string("\b");
    for (i = 0; i <= 20; i++)
        put_string(" ");
    for (i = 0; i <= 20; i++)
        put_string("\b");
} 
