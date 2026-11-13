    .syntax unified
    .cpu cortex-m3
    .thumb

    .global PendSV_Handler
    .thumb_func
    .type PendSV_Handler, %function
PendSV_Handler:
    mrs   r0, psp              @ r0 = current task's stack pointer
    ldr   r3, =current_tcb     @ r3 = address of the variable current_tcb
    ldr   r2, [r3]             @ r2 = current task's TCB
    cbz   r2, load_next        @ no current task yet? skip the saving part

    stmdb r0!, {r4-r11}        @ push R4-R11 on the current task's stack
    str   r0, [r2]             @ save SP into the current TCB (first field)

load_next:
    ldr   r1, =next_tcb
    ldr   r1, [r1]             @ r1 = next task's TCB
    str   r1, [r3]             @ current_tcb = next_tcb

    ldr   r0, [r1]             @ r0 = next task's saved SP
    ldmia r0!, {r4-r11}        @ pop R4-R11 of the next task
    msr   psp, r0              @ PSP now points at the next task's frame

    ldr   lr, =0xFFFFFFFD      @ "return to a task, use PSP"
    bx    lr                   @ hardware pops the other 8 registers

