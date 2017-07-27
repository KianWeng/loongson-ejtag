int tvsnprintf(char *buf, int n, const char *fmt, void **arg)
{
int i, l;
char c;
int w;
char *s;

for(i=0, l=0;fmt[i] && l<n;i++)
{
	c=fmt[i];
	if(c=='%')
	{
		w=1;
again:
		switch(fmt[i+1])
		{
			case 's':
				for(s = *arg;l<n && (c=*s); s++, l++)
					buf[l] = c;
				arg++;
				i++;
				break;
			case 'c':
				buf[l++]= *(char *)arg;
				arg++;
				i++;
				break;
			case 'u':
				l += snprintbase(buf + l, n-l, (long)*arg,w,10,0);
				arg++;
				i++;
				break;
			case 'd':
				l += snprintbase(buf + l, n-l, (long)*arg,w,10,1);
				arg++;
				i++;
				break;
			case 'o':
				l += snprintbase(buf + l, n-l, (long)*arg,w,8,0);
				arg++;
				i++;
				break;
			case 'b':
				l += snprintbase(buf + l, n-l, (long)*arg,w,2,0);
				arg++;
				i++;
				break;
			case 'p':
			case 'x':
				l += snprintbase(buf +l, n -l, (long)*arg,w,16,0);
				arg++;
				i++;
				break;
			case '%':
				buf[l++]='%';
				i++;
				break;
			case '0':
				i++;
			case '1' ... '9':
				for(w=0;fmt[i+1]>'0' && fmt[i+1]<='9';i++)
				 w=w*10+(fmt[i+1]-'0');
				goto again;
				 break;

			default:
				buf[l++]='%';
				break;
		}

	}
	else{
		buf[l++]=c;
	}
}
	buf[l++] = 0;
	return l;

}

