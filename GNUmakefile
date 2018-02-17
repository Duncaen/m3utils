CFLAGS = -g -Ilibtags/src
PROGS = m3print m3sort
ALL = $(PROGS)

LIBTAGS_CFLAGS = \
	-D__unix__\
	-Duint="unsigned int"\
	-Dushort="unsigned short"

LIBTAGS_OBJS =\
	libtags/src/8859.o\
	libtags/src/flac.o\
	libtags/src/id3genres.o\
	libtags/src/id3v1.o\
	libtags/src/id3v2.o\
	libtags/src/m4a.o\
	libtags/src/tags.o\
	libtags/src/utf16.o\
	libtags/src/vorbis.o

all : $(ALL)

$(PROGS) : % : %.o
m3print m3sort : m3u.o tags.o
m3print m3sort : $(LIBTAGS_OBJS)

README: man/m3utils.7
	mandoc -Tutf8 $< | col -bx >$@

clean:
	rm -f $(ALL) $(LIBTAGS_OBJS) *.o


$(LIBTAGS_OBJS): | libtags/src/tagspriv.h
$(LIBTAGS_OBJS): CFLAGS+=$(LIBTAGS_CFLAGS)
