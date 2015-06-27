#include <AT91RM9200_inc.h>

//------------------------------------------------------------------------------
// Arm startup code
// section
.section start
    .text
// modes defenition
#define ARM_MODE_USER   0x10
#define ARM_MODE_FIQ    0x11
#define ARM_MODE_IRQ    0x12
#define ARM_MODE_SVC    0x13
#define ARM_MODE_ABORT  0x17
#define ARM_MODE_UNDEF  0x1B
#define ARM_MODE_SYS    0x1F

#define I_BIT           0x80
#define F_BIT           0x40
#define T_BIT           0x20

// align 4 bytes (for ARM arch)
.align 4
// entry code point
    .globl _start
_start:
// Exception vectors (before Remap)
    b    reset           // Reset
    b    undefvec        // Undefined Instruction
    b    swivec          // Software Interrupt
    b    pabtvec         // Prefetch Abort
    b    dabtvec         // Data Abort
    b    rsvdvec         // restrved
    b    aicvec          // IRQ: read the AIC
    b    fiqvec          // FIQ
undefvec:
swivec:
pabtvec:
dabtvec:
rsvdvec:
aicvec:
fiqvec:
    b    undefvec
    
// Reset
reset:
#define MEMEND 0x00004000
// setup stack for each mode
#define IRQ_STACK_SIZE  0x10
#define FIQ_STACK_SIZE  0x04
#define ABT_STACK_SIZE  0x04
#define UND_STACK_SIZE  0x04
#define SVC_STACK_SIZE  0x10
#define USER_STACK_SIZE 0x400
    ldr    r0, = MEMEND
// Set up Supervisor Mode and set Supervisor Mode Stack
    msr    CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    mov    r13, r0      // Init stack Undef
    sub    r0, r0, #SVC_STACK_SIZE
    
// Set up Interrupt Mode and set IRQ Mode Stack
    msr    CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov    r13, r0      // Init stack IRQ
    sub    r0, r0, #IRQ_STACK_SIZE
    
// Set up Fast Interrupt Mode and set FIQ Mode Stack
    msr    CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
    mov    r13, r0      // Init stack FIQ
    sub    r0, r0, #FIQ_STACK_SIZE

// Set up Abort Mode and set Abort Mode Stack
    msr    CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT
    mov    r13, r0      // Init stack Abort
    sub    r0, r0, #ABT_STACK_SIZE

// Set up Undefined Instruction Mode and set Undef Mode Stack
    msr    CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT
    mov    r13, r0      // Init stack Undef
    sub    r0, r0, #UND_STACK_SIZE

// Set up User Mode adn set System Mode Stack
    msr    CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
    bic    r0, r0, #3   // Insur word alignement
    mov    sp, r0       // Init stack System

    ldr    r0, = low_level_init
    mov    lr, pc
    bx     r0

