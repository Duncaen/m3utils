#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3u.h"
#include "tags.h"

struct file {
	char *artist;
	char *album;
	char *title;
	int channels;
	int samplerate;
	int bitrate;
	int duration;
	int format;
};

struct aux {
	int fd;
	struct file *file;
};

long idx = 0;

static char buf[256];

static char default_fflag[] = "%a - %A: %t";
static char *fflag = default_fflag;

static int
ctxread(Tagctx *ctx, void *buf, int n)
{
	struct aux *a = ctx->aux;
	return read(a->fd, buf, n);
}

static int
ctxseek(Tagctx *ctx, int offset, int whence)
{
	struct aux *a = ctx->aux;
	return lseek(a->fd, offset, whence);
}

static void
cb(Tagctx *ctx, int t, const char *v, int offset, int size, Tagread f)
{
	struct file *fp = ((struct aux *)ctx->aux)->file;
	switch (t) {
	case Tartist: fp->artist = strdup(v); break;
	case Talbum: fp->album = strdup(v); break;
	case Ttitle: fp->title = strdup(v); break;
	}
}

struct aux aux;
Tagctx ctx = {
	.read = ctxread,
	.seek = ctxseek,
	.tag = cb,
	.buf = buf,
	.bufsz = sizeof (buf),
	.aux = &aux,
};

void
oneline(char *f)
{
	struct file fi;
	if ((aux.fd = open(f, O_RDONLY)) < 0) {
		fprintf(stderr, "open: %s\n", strerror(errno));
		return;
	}
	aux.file = &fi;
	ctx.filename = f;
	if (tagsget(&ctx)) {
		fprintf(stderr, "failed to get tags for: %s\n", f);
		close(aux.fd);
		return;
	}
	close(aux.fd);

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
			printf("%s", fi.artist);
			break;
		case 'A':
			printf("%s", fi.album);
			break;
		case 't':
			printf("%s", fi.title);
			break;
		case 'd':
			printf("%d", ctx.duration);
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
