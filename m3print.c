#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3utils.h"

long idx = 0;

static char default_fflag[] = "%-3i %T - %a: %t";
static char *fflag = default_fflag;

void
oneline(char *f)
{
	struct tags t;

	if (*f == '#')
		return;

	if (m3tags(f, &t) == -1)
		return;

	idx++;

	char *p;
	for (p = fflag; *p; p++) {
		if (*p == '\\') {
			switch (*++p) {
			case 'n': putchar('\n'); break;
			case 't': putchar('\t'); break;
			default:
				putchar('\\');
				putchar(*p);
			}
			continue;
		}
		if (*p != '%') {
			putchar(*p);
			continue;
		}
		p++;

		int w = 0;
		if ((*p >= '0' && *p <= '9') || *p == '-') {
			errno = 0;
			char *e;
			w = strtol(p, &e, 10);
			if (errno != 0)
				w = 0;
			else
				p = e;
		}

		if (!*p)
			break;

		switch (*p) {
		case '%':
			putchar('%');
			break;
		case 'f':
			printf("%*s", w, f);
			break;
		case 'i':
			printf("%*ld", w, idx);
			break;
		case 'T':
			printf("%*s", w, t.album);
			break;
		case 'A':
			printf("%*s", w, t.albumartist);
			break;
		case 'a':
			printf("%*s", w, t.artist);
			break;
		case 't':
			printf("%*s", w, t.title);
			break;
		case 'n':
			printf("%*s", w, t.track);
			break;
		case 'd':
			printf("%*d", w, t.duration);
			break;
		}
	}
	putchar('\n');
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
			    "Usage: m3print [-f format]\n");
			exit(1);
		}

	if (argc == optind && isatty(0))
		return m3u_loop(argc-optind, argv+optind, oneline);
	return m3u_loop(argc-optind, argv+optind, oneline);
}
