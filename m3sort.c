#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "m3utils.h"

struct file {
	long idx;
	char *file;
	struct tags tags;
};

struct file *files;
ssize_t filealloc = 1024;
long idx = 0;

int rflag = 0;

static int (*sortorders[16])(const void *, const void *);
int order_idx;

int
albumorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	return strcmp(ia->album, ib->album);
}

int
albumartistorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	return strcmp(ia->albumartist, ib->albumartist);
}

int
artistorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	return strcmp(ia->artist, ib->artist);
}

int
titleorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	return strcmp(ia->title, ib->title);
}

int
channelsorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	if (ia->channels > ib->channels) return 1;
	else if (ia->channels < ib->channels) return -1;
	else return 0;
}

int
samplerateorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	if (ia->samplerate > ib->samplerate) return 1;
	else if (ia->samplerate < ib->samplerate) return -1;
	else return 0;
}

int
bitrateorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	if (ia->bitrate > ib->bitrate) return 1;
	else if (ia->bitrate < ib->bitrate) return -1;
	else return 0;
}

int
durationorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	if (ia->duration > ib->duration) return 1;
	else if (ia->duration < ib->duration) return -1;
	else return 0;
}

int
formatorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;
	if (ia->format > ib->format) return 1;
	else if (ia->format < ib->format) return -1;
	else return 0;
}

int
trackorder(const void *a, const void *b)
{
	struct tags *ia = &((struct file *)a)->tags;
	struct tags *ib = &((struct file *)b)->tags;

	if (!ia->trackn) {
		errno = 0;
		ia->trackn = strtol(ia->track, NULL, 10);
		if (errno != 0)
			ia->trackn = -1;
	}
	if (!ib->trackn) {
		errno = 0;
		ib->trackn = strtol(ib->track, NULL, 10);
		if (errno != 0)
			ia->trackn = -1;
	}

	if (ia->trackn > ib->trackn) return 1;
	else if (ia->trackn < ib->trackn) return -1;
	else return 0;
}

int
idxorder(const void *a, const void *b)
{
	struct file *ia = (struct file *)a;
	struct file *ib = (struct file *)b;
	if (ia->idx > ib->idx) return 1;
	else if (ia->idx < ib->idx) return -1;
	else return 0;
}

int
order(const void *a, const void *b)
{
	int i, r;
	for (i = 0, r = 0; i < order_idx; i++) {
		r = (sortorders[i])(a, b);
		if (r != 0)
			return r;
	}
	return idxorder(a, b);
}

void
add(char *f)
{
	if (*f == '#')
		return;

	if (idx >= filealloc) {
		filealloc *= 2;
		if (filealloc < 0)
			exit(-1);
		files = realloc(files, sizeof (struct file) * filealloc);
		if (!files)
			exit(-1);
		memset(files+filealloc/2, 0, sizeof (struct file) * filealloc/2);
	}

	if (!files)
		exit(-1);

	files[idx].file = strdup(f);
	files[idx].idx = idx;

	if (m3tags(f, &files[idx].tags) == -1)
		return;

	idx++;
}

void
addorder(int (*fn)(const void *, const void *))
{
	if (order_idx < (int)(sizeof sortorders / sizeof sortorders[0]))
		sortorders[order_idx++] = fn;
}

int
main(int argc, char *argv[])
{
	int c, i;
	while ((c = getopt(argc, argv, "aAdnrtT")) != -1)
		switch (c) {
		case 'a': addorder(artistorder); break;
		case 'A': addorder(albumartistorder); break;
		case 'd': addorder(durationorder); break;
		case 'n': addorder(trackorder); break;
		case 'r': rflag = 1; break;
		case 't': addorder(titleorder); break;
		case 'T': addorder(albumorder); break;
		default:
			fprintf(stderr,
			    "Usage: m3sort [-r] [-adntA]\n");
			exit(1);
		}

	files = calloc(sizeof (struct file), filealloc);
	if (!files)
		exit(-1);

	if (argc == optind && isatty(0))
		m3u_loop(argc-optind, argv+optind, add);
	else
		m3u_loop(argc-optind, argv+optind, add);

	qsort(files, idx, sizeof (struct file), order);
	if (rflag)
		for (i = idx-1; i >= 0; i--)
			printf("%s\n", files[i].file);
	else
		for (i = 0; i < idx; i++)
			printf("%s\n", files[i].file);
}
