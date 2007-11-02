INCLUDES			= -I../../Common/include

bin_PROGRAMS		= uvcdynctrl
uvcdynctrl_SOURCES	= cmdline.c controls.c main.c
uvcdynctrl_CPPFLAGS	= -Wall --std=gnu99
uvcdynctrl_LDADD	= $(top_srcdir)/Libs/libwebcam/libwebcam.la
uvcdynctrl_LDFLAGS	= 
CLEANFILES			= cmdline.c cmdline.h

cmdline.c:
	gengetopt -i uvcdynctrl.ggo --unamed-opts=VALUES
