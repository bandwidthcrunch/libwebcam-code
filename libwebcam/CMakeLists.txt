#
# GENERAL
#

# Require CMake 2.6 because of the Debian packaging.
cmake_minimum_required (VERSION 2.6)

project (LIBWEBCAM)



#
# TARGETS
#

add_library (webcam SHARED libwebcam.c dynctrl.c)

set_target_properties (webcam PROPERTIES VERSION 0.2.0)



#
# COMPILATION AND LINKING
#

# Require LibXML
#include (FindLibXml2)
include (FindPkgConfig)
pkg_check_modules (LIBXML2 REQUIRED libxml-2.0)

# Check V4L2
try_compile (HAS_V4L2_STRING_CONTROLS ${CMAKE_BINARY_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/../common/build/cmake_try_v4l2_ctrl_type_string.c)
if (HAS_V4L2_STRING_CONTROLS)
	message("** Your V4L2 has V4L2_CTRL_TYPE_STRING support. Enabling raw control support.")
	add_definitions (-DENABLE_RAW_CONTROLS)
else (HAS_V4L2_STRING_CONTROLS)
	message("** Your V4L2 does not have V4L2_CTRL_TYPE_STRING support. Compiling without raw control support.")
endif (HAS_V4L2_STRING_CONTROLS)

# Includes
include_directories (include ${LIBXML2_INCLUDE_DIRS} ../common/include)

# Libraries
target_link_libraries (webcam ${LIBXML2_LIBRARIES})

# Compiler flags
set_target_properties (webcam PROPERTIES
	COMPILE_FLAGS "-Wall"
)



#
# INSTALLATION
#

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
	SET(CMAKE_INSTALL_LIBDIR "lib")
endif (NOT DEFINED CMAKE_INSTALL_LIBDIR)

if (${CMAKE_INSTALL_LIBDIR} MATCHES "^/")
	message("** Installation directory for libwebcam: ${CMAKE_INSTALL_LIBDIR}")
else (${CMAKE_INSTALL_LIBDIR} MATCHES "^/")
	message("** Installation directory for libwebcam: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif (${CMAKE_INSTALL_LIBDIR} MATCHES "^/")
install (
	TARGETS					webcam
	LIBRARY DESTINATION		${CMAKE_INSTALL_LIBDIR}
	COMPONENT				LIBWEBCAM
)
