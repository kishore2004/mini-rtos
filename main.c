#define UART_DATA    (*(volatile unsigned int *)0x40004000)
#define UART_STATE   (*(volatile unsigned int *)0x40004004)
#define UART_CTRL    (*(volatile unsigned int *)0x40004008)
#define UART_BAUDDIV (*(volatile unsigned int *)0x40004010)

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

void main(void)
{
    uart_init();
    uart_puts("Hello from my own bare-metal code!\r\n");
    while (1);
}
