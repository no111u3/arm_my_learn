	.cpu arm9tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.align	2
	.global	put_char
	.type	put_char, %function
put_char:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
.L2:
	ldr	r3, .L4
	ldr	r3, [r3, #20]
	and	r3, r3, #2
	cmp	r3, #0
	beq	.L2
	ldr	r3, .L4
	ldr	r2, [fp, #-8]
	and	r2, r2, #255
	str	r2, [r3, #28]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L5:
	.align	2
.L4:
	.word	-3584
	.size	put_char, .-put_char
	.align	2
	.global	get_char
	.type	get_char, %function
get_char:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
.L7:
	ldr	r3, .L9
	ldr	r3, [r3, #20]
	and	r3, r3, #1
	cmp	r3, #0
	beq	.L7
	ldr	r3, .L9
	ldr	r3, [r3, #24]
	and	r3, r3, #255
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.L10:
	.align	2
.L9:
	.word	-3584
	.size	get_char, .-get_char
	.align	2
	.global	put_string
	.type	put_string, %function
put_string:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	b	.L12
.L13:
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	ldr	r2, [fp, #-8]
	add	r2, r2, #1
	str	r2, [fp, #-8]
	mov	r0, r3
	bl	put_char
.L12:
	ldr	r3, [fp, #-8]
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	bne	.L13
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	put_string, .-put_string
	.align	2
	.global	get_string
	.type	get_string, %function
get_string:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-20]
	add	r3, r2, r3
	str	r3, [fp, #-12]
	b	.L16
.L18:
	bl	get_char
	mov	r3, r0
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-24]
	cmp	r3, #0
	beq	.L17
	ldr	r0, [fp, #-8]
	bl	put_char
.L17:
	ldr	r3, [fp, #-8]
	and	r2, r3, #255
	ldr	r3, [fp, #-16]
	strb	r2, [r3, #0]
	ldr	r3, [fp, #-16]
	add	r3, r3, #1
	str	r3, [fp, #-16]
.L16:
	ldr	r2, [fp, #-16]
	ldr	r3, [fp, #-12]
	cmp	r2, r3
	blt	.L18
	ldr	r3, [fp, #-16]
	mov	r2, #0
	strb	r2, [r3, #0]
	sub	sp, fp, #4
	ldmfd	sp!, {fp, lr}
	bx	lr
	.size	get_string, .-get_string
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	ldr	r3, .L23
	mov	r2, #134217728
	str	r2, [r3, #52]
.L21:
	ldr	r3, .L23+4
	mov	r2, #1
	str	r2, [r3, #4]
	b	.L21
.L24:
	.align	2
.L23:
	.word	-2560
	.word	-1024
	.size	main, .-main
	.ident	"GCC: (Gentoo 4.4.5 p1.3, pie-0.4.5) 4.4.5"
	.section	.note.GNU-stack,"",%progbits
