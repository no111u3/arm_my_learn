/**
 * AIC init
 */
#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xe51fff20) // ldr, pc, [pc, #-&F20]
// AIC enable irq
void aic_enable_irq (unsigned int irq_id) {
    // enable the interrupt on the interrupt controller
    AT91C_BASE_AIC->AIC_IECR = 0x1 << irq_id ;
}
// AIC disable irq
void aic_disable_irq(unsigned int irq_id) {
    int mask = 0x1 << irq_id;
    
    // disable the interrupt on the interrupt controller
    AT91C_BASE_AIC->AIC_IDCR = mask ;
    // clear the interrupt on the Interrupt Controller ( if one is pending )
    AT91C_BASE_AIC->AIC_ICCR = mask ;
}
// AIC configure irq
unsigned aic_configure_irq (
    unsigned int irq_id,     // \arg interrupt number to initialize
    unsigned int priority,   // \arg priority to give to the interrupt
    unsigned int src_type,   // \arg activation and sense of activation
    void (*newHandler)(void) ) {// \arg address of the interrupt handler
    unsigned int oldHandler;
    unsigned int mask ;

    oldHandler = AT91C_BASE_AIC->AIC_SVR[irq_id];

    mask = 0x1 << irq_id ;
    //* Disable the interrupt on the interrupt controller
    AT91C_BASE_AIC->AIC_IDCR = mask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    AT91C_BASE_AIC->AIC_SVR[irq_id] = (unsigned int) newHandler ;
    //* Store the Source Mode Register
    AT91C_BASE_AIC->AIC_SMR[irq_id] = src_type | priority  ;
    //* Clear the interrupt on the interrupt controller
    AT91C_BASE_AIC->AIC_ICCR = mask ;

    return oldHandler;
}
// AIC set exception vector
unsigned aic_set_exception_vector(unsigned int *pVector, // \arg pointer to the AIC registers
    void (*Handler) () ) { // \arg Interrupt Handler
    int oldVector = *pVector;

    if ((int) Handler == (unsigned int) AT91C_AIC_BRANCH_OPCODE)
        *pVector = (unsigned int) AT91C_AIC_BRANCH_OPCODE;
    else
        *pVector = (((((unsigned int) Handler) - ((unsigned int) pVector) - 0x8) >> 2) & 0x00ffffff) | 0xEA000000;

    return oldVector;
}
// AIC acknowledge irq
void aic_acknowledge_irq () { // \arg pointer to the AIC registers
    AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_AIC->AIC_EOICR;
}
// AIC init
void aic_init(void (*IrqHandler) (),  // \arg Default IRQ vector exception
	void (*FiqHandler) (),  // \arg Default FIQ vector exception
	void (*DefaultHandler)  (), // \arg Default Handler set in ISR
	void (*SpuriousHandler) (), // \arg Default Spurious Handler
	int protectMode) {   // \arg Debug Control Register
    
    int i;

	// disable all interrupts and set IVR to the default handler
	for (i = 0; i < 32; ++i) {
		aic_disable_irq(i);
		aic_configure_irq(i, AT91C_AIC_PRIOR_LOWEST,
            AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, DefaultHandler);
	}
    
    // set the IRQ exception vector
	aic_set_exception_vector((unsigned int *) 0x18, IrqHandler);
	// set the Fast Interrupt exception vector
	aic_set_exception_vector((unsigned int *) 0x1C, FiqHandler);

	AT91C_BASE_AIC->AIC_SPU = (unsigned int) SpuriousHandler;
	AT91C_BASE_AIC->AIC_DCR = protectMode;
}