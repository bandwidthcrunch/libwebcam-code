lib_LTLIBRARIES			= libwebcam.la
libwebcam_la_SOURCES	= libwebcam.c dynctrl.c
libwebcam_la_CPPFLAGS	= -Wall -Werror -I../../Common/include $(LIBXML2_CFLAGS)
libwebcam_la_LDFLAGS	= $(LIBXML2_LIBS) -version-info ${LIBWEBCAM_INTERFACE_VERSION} -release ${PACKAGE_VERSION} -Wl,--version-script=dynamic.lst -L/usr/lib

bin_PROGRAMS			= test
test_SOURCES			= test.c
test_CPPFLAGS			= -Wall -std=gnu99 -I../../Common/include
test_LDFLAGS			= -lpthread -lwebcam
