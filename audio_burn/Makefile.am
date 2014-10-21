bin_PROGRAMS = audio_burn

audio_burn_SOURCES = audio_burn.c

AM_CFLAGS = @CFLAGS@
AM_CPPFLAGS = @CPPFLAGS@

audio_burn_FLAGS = ($CXXFLAGS)

INCLUDES = -I../libaudioburn

audio_burn_LDADD = -L../libaudioburn -laudioburn \
                   -L/usr/lib -lvorbisfile -lvorbis

