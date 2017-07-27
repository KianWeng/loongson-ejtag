int getchar()
{
return *(volatile char *)SERIAL_REG;
}
