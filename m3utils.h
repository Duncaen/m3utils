// tags.c
struct tags {
	char *artist;
	char *album;
	char *title;
	int channels;
	int samplerate;
	int bitrate;
	int duration;
	int format;
};

int m3tags(char *, struct tags *);

// m3u.c
long m3u_loop(int, char **, void(*)(char *));
