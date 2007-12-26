ADD_EXECUTABLE (uvcdynctrl main.c controls.c cmdline.c)

ADD_CUSTOM_COMMAND(
	OUTPUT		${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c ${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h
	COMMAND		gengetopt ARGS -i ${CMAKE_CURRENT_SOURCE_DIR}/uvcdynctrl.ggo --output-dir ${CMAKE_CURRENT_SOURCE_DIR} --unamed-opts=VALUES
	DEPENDS 	${CMAKE_CURRENT_SOURCE_DIR}/uvcdynctrl.ggo
	COMMENT		"Generating getopt parser code (cmdline.*) ..."
	VERBATIM
)

INCLUDE_DIRECTORIES(${LIBWEBCAM_SOURCE_DIR}/Common/include)
LINK_DIRECTORIES (${LIBWEBCAM_BINARY_DIR}/webcam)

TARGET_LINK_LIBRARIES(uvcdynctrl webcam)

ADD_DEFINITIONS(-Wall --std=gnu99)
