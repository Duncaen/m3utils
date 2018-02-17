#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3utils.h"

enum {
	Tartist = 1,
	Talbum = 2,
	Ttitle = 3,
};

static int iflag = 0;

static long matches = 0;
static int tag = 0;

static regex_t pattern;

int
match(char *s)
{
	regmatch_t pmatch;
	while (*s && regexec(&pattern, s, 1, &pmatch, 0) == 0) {
		matches++;
		return 1;
	}
	return 0;
}

void
oneline(char *f)
{
	struct tags t;
	char *s;

	if (m3tags(f, &t) == -1)
		return;

	switch (tag) {
	case Tartist: s = t.artist; break;
	case Talbum: s = t.album; break;
	case Ttitle: s = t.title; break;
	}

	if (match(s))
		printf("%s\n", f);
}

int
main(int argc, char *argv[])
{
	int c, i;
	while ((c = getopt(argc, argv, "i")) != -1)
		switch (c) {
		case 'i': iflag = REG_ICASE; break;
		default:
usage:
			fprintf(stderr,
			    "Usage: m3grep [-i] tag:regex\n");
			exit(1);
		}

	if (argc == optind)
		goto usage;
	char *s = argv[optind++];
	char *rx = strchr(s, ':');
	if (!rx)
		goto usage;
	*rx++ = '\0';

	if (!strcmp(s, "artist"))
		tag = Tartist;
	else if (!strcmp(s, "album"))
		tag = Talbum;
	else if (!strcmp(s, "title"))
		tag = Ttitle;

	int r = regcomp(&pattern, rx, REG_EXTENDED | iflag);
	if (r != 0) {
		char buf[256];
		regerror(r, &pattern, buf, sizeof buf);
		fprintf(stderr, "m3grep: regex error: %s\n", buf);
		exit(2);
	}

	if (argc == optind && isatty(0))
		m3u_loop(argc-optind, argv+optind, oneline);
	else
		m3u_loop(argc-optind, argv+optind, oneline);

	return !matches;
}
