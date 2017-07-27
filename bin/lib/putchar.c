int putchar(int c)
{
*(volatile char *)SERIAL_REG=c;
return 0;
}
