#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3utils.h"

long idx = 0;

static char default_fflag[] = "%a - %A: %t";
static char *fflag = default_fflag;

void
oneline(char *f)
{
	struct tags t;
	if (m3tags(f, &t) == -1)
		return;

	char *p;
	for (p = fflag; *p; p++) {
		if (*p == '\\') {
			p++;
			continue;
		}
		if (*p != '%') {
			putchar(*p);
			continue;
		}
		p++;
		switch (*p) {
		case '%':
			putchar('%');
			break;
		case 'a':
			printf("%s", t.artist);
			break;
		case 'A':
			printf("%s", t.album);
			break;
		case 't':
			printf("%s", t.title);
			break;
		case 'd':
			printf("%d", t.duration);
			break;
		}
	}
	putchar('\n');
	idx++;
}

int
main(int argc, char *argv[])
{
	int c, i;
	while ((c = getopt(argc, argv, "f:")) != -1)
		switch (c) {
		case 'f': fflag = optarg; break;
		default:
			fprintf(stderr,
			    "Usage: m3print\n");
			exit(1);
		}

	if (argc == optind && isatty(0))
		return m3u_loop(argc-optind, argv+optind, oneline);
	return m3u_loop(argc-optind, argv+optind, oneline);
}
