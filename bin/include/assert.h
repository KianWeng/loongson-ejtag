#define assert(p) do {	\
	if (!(p)) {	\
		printf("BUG at %s:%d assert(%s)\n",	\
		       __FILE__, __LINE__, #p);			\
		while(1);	\
	}		\
} while (0)
