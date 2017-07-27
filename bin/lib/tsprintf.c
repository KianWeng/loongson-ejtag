int tsprintf(char *buf, const char *fmt,...)
{
	void **arg;
	void *ap;
        int l;

	__builtin_va_start(ap,fmt);
	arg=ap;
	l = tvsnprintf(buf, 0x7fffffff, fmt, arg);
	return l;
}

