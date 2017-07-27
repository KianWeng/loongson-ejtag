unsigned int 
strlen(const char *p)
{
	int n;

	if (!p)
		return (0);
	for (n = 0; *p; p++)
		n++;
	return (n);
}
