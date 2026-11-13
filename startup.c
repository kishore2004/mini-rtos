extern unsigned int _estack, _etext, _sdata, _edata, _sbss, _ebss;
void main(void);

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
    &_estack,
    Reset_Handler,
    Default_Handler,
    Default_Handler,
};
