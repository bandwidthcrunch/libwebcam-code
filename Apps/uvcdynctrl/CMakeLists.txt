#
# TARGETS
#

ADD_EXECUTABLE (uvcdynctrl main.c controls.c cmdline.c)


#
# SPECIAL FILES
#

# gengetopt for command line parsing
ADD_CUSTOM_COMMAND (
	OUTPUT		${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h
	COMMAND		gengetopt ARGS -i ${CMAKE_CURRENT_SOURCE_DIR}/uvcdynctrl.ggo --output-dir ${CMAKE_CURRENT_SOURCE_DIR} --unamed-opts=VALUES
	DEPENDS 	${CMAKE_CURRENT_SOURCE_DIR}/uvcdynctrl.ggo
	COMMENT		"Generating getopt parser code (cmdline.*) ..."
	VERBATIM
)


#
# COMPILATION AND LINKING
#

ADD_DEFINITIONS (-Wall --std=gnu99)

INCLUDE_DIRECTORIES (${LIBWEBCAM_SOURCE_DIR}/Common/include)
LINK_DIRECTORIES (${LIBWEBCAM_BINARY_DIR}/webcam)

TARGET_LINK_LIBRARIES (uvcdynctrl webcam)


#
# INSTALLATION
#

# Tool
INSTALL (
	TARGETS		uvcdynctrl
	RUNTIME DESTINATION	bin
)

# udev rules, script, and data
# Default permissions for DIRECTORY files: rw-r--r--
# Default permissions for PROGRAMS files:  rwxr-xr-x
INSTALL (
	DIRECTORY	udev/rules/
	DESTINATION /etc/udev/rules.d
	PATTERN ".svn" EXCLUDE
)
INSTALL (
	PROGRAMS	udev/scripts/uvcdynctrl
	DESTINATION	/lib/udev
)
INSTALL (
	DIRECTORY	data
	DESTINATION /etc/udev
	PATTERN ".svn" EXCLUDE
)
