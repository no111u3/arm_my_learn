/**
 * AIC interrupt handlers
 */
// AIC spirious handler
void aic_spirious_int(void) {
    put_string("Spirious Interrupt detected\n");
    while (1);
}
// AIC data abort handler
void aic_data_abort(void) {
    put_string("Data Abort detected\n");
    while (1);
}
// AIC fetch abort handler
void aic_fetch_abort(void) {
    put_string("Fetch Abort detected\n");
    while (1);
}
// AIC undefined handler
void aic_undefined(void) {
    put_string("Udefined detected\n");
    while (1);
}
// AIC default handler
void aic_default(void) {
    put_string("Handler not install use default");
    while (1);
}
