int tsnprintf(char *buf, int n, const char *fmt,...)
{
	void **arg;
	void *ap;
        int l;

	__builtin_va_start(ap,fmt);
	arg=ap;
	l = tvsnprintf(buf, n, fmt, arg);
	return l;
}

