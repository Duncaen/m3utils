#include <stdlib.h>
#include <stdio.h>

long
m3u_loop(int argc, char *argv[], void (*cb)(char *))
{
	char *line = 0;
	size_t linelen = 0;
	ssize_t rd;
	long i = 0;

	if (argc == 0) {
		while ((rd = getdelim(&line, &linelen, '\n', stdin)) != -1) {
			if (line[rd-1] == '\n')
				line[rd-1] = '\0';
			cb(line);
			i++;
		}
		/* free(line); */
		return i;
	}
	return 0;
}
