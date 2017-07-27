int snprintbase(char *d, int n, long v,int w,int base,int sign)
{
	int i,j;
	int c;
	char buf[64];
	unsigned long value;
	int l = 0;
	if(sign && v<0)
	{
	value = -v;
	d[l++]='-';
	}
	else value=v;

	for(i=0;value;i++)
	{
	buf[i]=value%base;
	value=value/base;
	}

#define max(a,b) (((a)>(b))?(a):(b))

	for(j=max(w,i);j>0 && l<n;j--)
	{
		c=j>i?0:buf[j-1];
		d[l++] = (c<=9)?c+'0':c-0xa+'a';
	}
	return l;
}

int sprintbase(char *d, long v,int w,int base,int sign)
{
 return snprintbase(d, 0x7fffffff, v, w, base, sign);
}
