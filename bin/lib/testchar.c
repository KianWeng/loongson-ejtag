int testchar()
{
return *(volatile char *)(SERIAL_REG+1);
}
