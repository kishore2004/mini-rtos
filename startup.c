extern unsigned int _estack, _etext, _sdata, _edata, _sbss, _ebss;
void main(void);

void PendSV_Handler(void);   /* written in assembly */
void SysTick_Handler(void);  /* written in main.c   */

void Reset_Handler(void)
{
    unsigned int *src = &_etext;
    unsigned int *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    for (dst = &_sbss; dst < &_ebss; dst++) *dst = 0;

    main();
    while (1);
}

void Default_Handler(void) { while (1); }

__attribute__((section(".vectors"), used))
const void *vector_table[] = {
    &_estack,          /* 0  initial stack pointer */
    Reset_Handler,     /* 1  reset                 */
    Default_Handler,   /* 2  NMI                   */
    Default_Handler,   /* 3  HardFault             */
    Default_Handler,   /* 4  MemManage             */
    Default_Handler,   /* 5  BusFault              */
    Default_Handler,   /* 6  UsageFault            */
    0, 0, 0, 0,        /* 7-10 reserved            */
    Default_Handler,   /* 11 SVCall                */
    Default_Handler,   /* 12 DebugMon              */
    0,                 /* 13 reserved              */
    PendSV_Handler,    /* 14 PendSV                */
    SysTick_Handler,   /* 15 SysTick               */
};
