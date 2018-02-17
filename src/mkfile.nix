LIB=libtags.a

<$MKRULES/defaults

OFILES=\
	8859.$O\
	flac.$O\
	id3genres.$O\
	id3v1.$O\
	id3v2.$O\
	m4a.$O\
	tags.$O\
	utf16.$O\
	vorbis.$O\

%.$O: tagspriv.h

HFILES=\
	tags.h\

<$MKRULES/mklib
