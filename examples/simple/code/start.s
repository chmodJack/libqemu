.global _start
.section .text,"ax"
_start:
	b reset
	ldr pc,_undefined_instruction
	ldr pc,_software_interrupt
	ldr pc,_prefetch_abort
	ldr pc,_data_abort
	ldr pc,_not_used
	ldr pc,_irq
	ldr pc,_fiq

_undefined_instruction:.word undefined_instruction
_software_interrupt:   .word software_interrupt
_prefetch_abort:       .word prefetch_abort
_data_abort:           .word data_abort
_not_used:             .word not_used
_irq:                  .word irq
_fiq:                  .word fiq

.global undefined_instruction
.global software_interrupt
.global prefetch_abort
.global data_abort
.global not_used
.global irq
.global fiq

undefined_instruction:
	b .
software_interrupt:
	b .
prefetch_abort:
	b .
data_abort:
	b .
not_used:
	b .
irq:
	ldr sp,irq_sp
	push {r0-r12,lr}
	bl do_irq
	pop {r0-r12,lr}
	subs pc,lr,#4
fiq:
	ldr sp,fiq_sp
	push {r0-r12,lr}
	bl do_fiq
	pop {r0-r12,lr}
	subs pc,lr,#4

.global reset
.section .text
reset:
	mov r0, lr
	mov r1, #0x0000
	mov r2, #0x0000
	mov r3, #0x0000
	mov r4, #0x0000
	mov r5, #0x0000
	mov r6, #0x0000
	mov r7, #0x0000
	mov r8, #0x0000
	mov r9, #0x0000
	mov r10, #0x0000
	mov r11, #0x0000
	mov r12, #0x0000
	mov r13, #0x0000

	mrs r1, cpsr
	msr spsr_cxsf, r1
	@; Switch to FIQ mode (M = 10001)
	cps #17
	mov lr, r0
	mov r8, #0x0000
	mov r9, #0x0000
	mov r10, #0x0000
	mov r11, #0x0000
	mov r12, #0x0000
	mrs r1, cpsr
	msr spsr_cxsf, r1
	@; Switch to IRQ mode (M = 10010)
	cps #18
	mov lr, r0
	mrs r1,cpsr
	msr spsr_cxsf, r1
	@; Switch to Abort mode (M = 10111)
	cps #23
	mov lr, r0
	mrs r1,cpsr
	msr spsr_cxsf, r1
	@; Switch to Undefined Instruction Mode (M = 11011)
	cps #27
	mov lr, r0
	mrs r1,cpsr
	msr spsr_cxsf, r1
	@; Switch to System Mode ( Shares User Mode registers ) (M = 11111)
	cps #31
	mov lr, r0
	mrs r1,cpsr
	msr spsr_cxsf, r1

	bl init_stack_pointer

	mrs   r0,  cpsr
	bicne r0,  r0,   #0x1f @clear all mode bits
	orrne r0,  r0,   #0x13 @set SVC mode
	bic   r0,  r0,   #0xc0 @enable IRQ and FIQ
	msr   cpsr,r0

	bl main

	cmp r0,#0
	beq reset
	b err

init_stack_pointer:
	cps   #17
	ldr   sp,       fiq_sp
	cps   #18
	ldr   sp,       irq_sp
	cps   #19
	ldr   sp,       svc_sp
	cps   #23
	ldr   sp,       abort_sp
	cps   #27
	ldr   sp,       undef_sp
	cps   #31
	ldr   sp,       user_sp
	bx    lr

user_sp:  .word 0xFFFFC - 0x00000100 - 0x00000100 - 0x00000100 - 0x00000100 - 0x00000100
svc_sp:   .word 0xFFFFC - 0x00000100 - 0x00000100 - 0x00000100 - 0x00000100
fiq_sp:   .word 0xFFFFC - 0x00000100 - 0x00000100 - 0x00000100
irq_sp:   .word 0xFFFFC - 0x00000100 - 0x00000100
abort_sp: .word 0xFFFFC - 0x00000100
undef_sp: .word 0xFFFFC

err:
b .
