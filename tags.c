#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "m3utils.h"
#include "tags.h"

static char buf[256];

struct aux {
	int fd;
	struct tags *t;
};

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
	struct tags *ta = ((struct aux *)ctx->aux)->t;
	switch (t) {
	case Tartist: ta->artist = strdup(v); break;
	case Talbum: ta->album = strdup(v); break;
	case Ttitle: ta->title = strdup(v); break;
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

int
m3tags(char *file, struct tags *t)
{
	if ((aux.fd = open(file, O_RDONLY)) < 0) {
		return -1;
	}
	aux.t = t;
	ctx.filename = file;
	if (tagsget(&ctx)) {
		close(aux.fd);
		return -1;
	}
	t->channels = ctx.channels;
	t->samplerate = ctx.samplerate;
	t->bitrate = ctx.bitrate;
	t->duration = ctx.duration;
	t->format = ctx.format;
	close(aux.fd);
	return 0;
}
