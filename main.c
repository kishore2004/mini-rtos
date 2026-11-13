#define UART_DATA    (*(volatile unsigned int *)0x40004000)
#define UART_STATE   (*(volatile unsigned int *)0x40004004)
#define UART_CTRL    (*(volatile unsigned int *)0x40004008)
#define UART_BAUDDIV (*(volatile unsigned int *)0x40004010)
/* Cortex-M system registers */
#define SYST_CTRL  (*(volatile unsigned int *)0xE000E010)
#define SYST_LOAD  (*(volatile unsigned int *)0xE000E014)
#define SYST_VAL   (*(volatile unsigned int *)0xE000E018)
#define ICSR       (*(volatile unsigned int *)0xE000ED04)
#define SHPR3      (*(volatile unsigned int *)0xE000ED20)
#define PENDSVSET  (1u << 28)

static void uart_init(void)
{
    UART_BAUDDIV = 16;
    UART_CTRL = 1;
}

static void uart_putc(char c)
{
    while (UART_STATE & 1);
    UART_DATA = c;
}

static void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);
}

typedef struct {
    unsigned int *sp;      /* MUST be the first field: assembly uses offset 0 */
} tcb_t;

#define NUM_TASKS   3
#define STACK_WORDS 256    /* 256 words = 1 KB per task */

static tcb_t tcbs[NUM_TASKS];
static unsigned int stack_a[STACK_WORDS] __attribute__((aligned(8)));
static unsigned int stack_b[STACK_WORDS] __attribute__((aligned(8)));
static unsigned int stack_c[STACK_WORDS] __attribute__((aligned(8)));

/* The assembly code reads and writes these two */
tcb_t * volatile current_tcb = 0;
tcb_t * volatile next_tcb    = 0;

static int cur = 0;        /* index of the task that runs now */

static void task_exit(void) { while (1); }

static void task_create(tcb_t *tcb, unsigned int *stack,
                        unsigned int words, void (*fn)(void))
{
    unsigned int *sp = stack + words;       /* top of the stack (it grows down) */

    *(--sp) = 0x01000000;                   /* xPSR: Thumb bit must be 1        */
    *(--sp) = (unsigned int)fn;             /* PC  : where the task starts      */
    *(--sp) = (unsigned int)task_exit;      /* LR                               */
    *(--sp) = 0;                            /* R12                              */
    *(--sp) = 0;                            /* R3                               */
    *(--sp) = 0;                            /* R2                               */
    *(--sp) = 0;                            /* R1                               */
    *(--sp) = 0;                            /* R0                               */
    for (int i = 0; i < 8; i++)             /* R11 ... R4 (saved by software)   */
        *(--sp) = 0;

    tcb->sp = sp;
}

static void delay(void)
{
    for (volatile unsigned int i = 0; i < 300000; i++);
}

static void task_a(void) { while (1) { uart_putc('A'); delay(); } }
static void task_b(void) { while (1) { uart_putc('B'); delay(); } }
static void task_c(void) { while (1) { uart_putc('C'); delay(); } }

void SysTick_Handler(void)
{
    cur = (cur + 1) % NUM_TASKS;     /* 0 -> 1 -> 2 -> 0 -> ...    */
    next_tcb = &tcbs[cur];
    ICSR = PENDSVSET;                /* ask for a switch (set PendSV) */
}

void main(void)
{
    uart_init();
    uart_puts("Mini RTOS starting...\r\n");

    task_create(&tcbs[0], stack_a, STACK_WORDS, task_a);
    task_create(&tcbs[1], stack_b, STACK_WORDS, task_b);
    task_create(&tcbs[2], stack_c, STACK_WORDS, task_c);

    SHPR3 |= 0xFFFF0000;             /* PendSV and SysTick = lowest priority */

    current_tcb = 0;                 /* no task is running yet               */
    next_tcb    = &tcbs[0];          /* first task: A                        */
    cur         = 0;

    SYST_LOAD = 1250000 - 1;         /* about 50 ms per tick                 */
    SYST_VAL  = 0;
    SYST_CTRL = 7;                   /* enable + interrupt + core clock      */

    ICSR = PENDSVSET;                /* first switch: start task A           */
    __asm volatile ("dsb");
    __asm volatile ("isb");

    while (1);                       /* never reached */
}