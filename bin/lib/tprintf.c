

#define N 256

int tprintf(const char *fmt,...)
{
	void **arg;
	void *ap;
	char buf[N];
        int l;

	__builtin_va_start(ap,fmt);
	arg=ap;
	l = tvsnprintf(buf, N, fmt, arg);
	putstring(buf);
	return l;
}
