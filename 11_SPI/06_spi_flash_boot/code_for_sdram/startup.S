#include <AT91RM9200_inc.h>
        
/*---------------------------
Code loaded in SDRAM
---------------------------*/
.section start
    .text

  .align  4
    .globl _start
_start:

            ldr     r0, = main
            mov     lr, pc
            bx      r0

End:
            b       End