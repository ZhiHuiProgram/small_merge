void *uart_getdata(int fd);
void *uart_putsdata(unsigned char *a,int len, int fd);
void *uart_putdata(unsigned char *a,int fd);
int uart_open(char *file, int baud_rate);
#define SERIAL_DEV "/dev/ttyS5"
#define BAUD_RATE 115200