#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3utils.h"

enum {
	Tall = 1,
	Talbum,
	Tartist,
	Ttitle,
};

static int cflag = 0;
static int iflag = 0;
static int mflag = 0;
static int oflag = 0;
static int pflag = 0;
static int qflag = 0;
static int vflag = 0;

static long matches = 0;
static int tag = Tall;

static regex_t pattern;

int
match(char *file, char *tag, char *s)
{
	if (oflag && !cflag && !qflag && !vflag) {
		regmatch_t pmatch;
		int len, matched = 0;
		while (*s && regexec(&pattern, s, 1, &pmatch, 0) == 0) {
			s += pmatch.rm_so;
			if (!(len = pmatch.rm_eo-pmatch.rm_so)) {
				s += 1;
				continue;
			}
			if (pflag)
				printf("%s: %s: ", file, tag);
			printf("%.*s\n", len, s);
			s += len;
			matched++;
		}
		return (matched && matches++);
	} else if (vflag ^ (regexec(&pattern, s, 0, 0, 0) == 0)) {
		if (qflag)
			exit(0);
		matches++;
		if (!cflag) {
			printf("%s", file);
			if (pflag && !vflag)
				printf(": %s: %s", tag, s);
			putchar('\n');
		}
		if (mflag && matches >= mflag)
			exit(0);
		return 1;
	}
	return 0;
}

void
grep(char *f)
{
	struct tags t;

	if (*f == '#')
		return;

	if (m3tags(f, &t) == -1)
		return;

	switch (tag) {
	case Talbum: match(f, "album", t.album); break;
	case Tartist: match(f, "artist", t.artist); break;
	case Ttitle: match(f, "title", t.title); break;
	case Tall:
		if (cflag) {
			(void)(match(f, "album", t.album) ||
			    match(f, "album", t.album) ||
			    match(f, "album", t.album));
			return;
		}
		match(f, "album", t.album);
		match(f, "artist", t.artist);
		match(f, "title", t.title);
	}
}

int
main(int argc, char *argv[])
{
	int c, i;
	while ((c = getopt(argc, argv, "cim:opqv")) != -1)
		switch (c) {
		case 'c': cflag = 1;
		case 'i': iflag = REG_ICASE; break;
		case 'm': mflag = atol(optarg); break;
		case 'o': oflag = 1; break;
		case 'p': pflag = 1; break;
		case 'q': qflag = 1; break;
		case 'v': vflag = 1; break;
		default:
usage:
			fprintf(stderr,
			    "Usage: m3grep [-i] [-m max] [tag:]regex\n");
			exit(1);
		}

	if (argc == optind)
		goto usage;
	char *s = argv[optind++];
	char *rx;
	if ((rx = strchr(s, ':'))) {
		*rx++ = '\0';
		if (!strcmp(s, "artist"))
			tag = Tartist;
		else if (!strcmp(s, "album"))
			tag = Talbum;
		else if (!strcmp(s, "title"))
			tag = Ttitle;
		else
			goto usage;
	}

	int r = regcomp(&pattern, rx ? rx : s, REG_EXTENDED | iflag);
	if (r != 0) {
		char buf[256];
		regerror(r, &pattern, buf, sizeof buf);
		fprintf(stderr, "m3grep: regex error: %s\n", buf);
		exit(2);
	}

	if (argc == optind && isatty(0))
		m3u_loop(argc-optind, argv+optind, grep);
	else
		m3u_loop(argc-optind, argv+optind, grep);

	if (cflag)
		printf("%ld\n", matches);

	return !matches;
}
