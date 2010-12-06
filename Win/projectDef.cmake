#/**********************************************************\ 
# Auto-generated Windows project definition file for the
# WebP2P project
#\**********************************************************/

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Win/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/[^.]*.cpp
    Win/[^.]*.h
    Win/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    /D "_ATL_STATIC_REGISTRY"
)

SOURCE_GROUP(Win FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_library(${PROJNAME} SHARED ${SOURCES})

set_target_properties (${PROJNAME} PROPERTIES
    OUTPUT_NAME np${PLUGIN_NAME}
    PROJECT_LABEL ${PROJNAME}
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/${PLUGIN_NAME}"
    )

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
find_library(WS2_32 Ws2_32.lib)
if(NOT WS2_32)
  message(FATAL_ERROR "Could not FIND ws2_32.lib")
endif()
find_library(PJ_LIB pjlib-x86-msvc100-Release.lib ..\\pjproject\\lib)
if(NOT PJ_LIB)
  message(FATAL_ERROR "Could not find pjlib-x86-msvc100-Release.lib")
endif()
find_library(PJ_LIB_UTIL pjlib_util-x86-msvc100-Release.lib ..\\pjproject\\lib)
if(NOT PJ_LIB_UTIL)
  message(FATAL_ERROR "Could not find pjlib_util-x86-msvc100-Release.lib")
endif()
find_library(PJ_NATH pjnath-x86-msvc100-Release.lib ..\\pjproject\\lib)
if(NOT PJ_NATH)
  message(FATAL_ERROR "Could not find pjnath-x86-msvc100-Release.lib")
endif()

target_link_libraries(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
	${WS2_32}
    ${PJ_LIB}
    ${PJ_LIB_UTIL}
    ${PJ_NATH}
    )

find_path(PJ_LIB_INCLUDES pjlib.h ..\\pjproject\\pjlib\\include)
if(NOT PJ_LIB_INCLUDES)
	message(FATAL_ERROR "Could not find pjlib.h")
endif(NOT PJ_LIB_INCLUDES)
include_directories(BEFORE ${PJ_LIB_INCLUDES})

find_path(PJ_LIB_UTIL_INCLUDES pjlib-util.h ..\\pjproject\\pjlib-util\\include)
if(NOT PJ_LIB_UTIL_INCLUDES)
	message(FATAL_ERROR "Could not find pjlib-util.h")
endif(NOT PJ_LIB_UTIL_INCLUDES)
include_directories(BEFORE ${PJ_LIB_UTIL_INCLUDES})

find_path(PJ_NATH_INCLUDES pjnath.h ..\\pjproject\\pjnath\\include)
if(NOT PJ_NATH_INCLUDES)
	message(FATAL_ERROR "Could not find pjnath.h")
endif(NOT PJ_NATH_INCLUDES)
include_directories(BEFORE ${PJ_NATH_INCLUDES})

set(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

add_wix_installer( ${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/ConnectionPeerInstaller.wxs
    PluginDLLGroup
    ${BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
    ${BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/np${PLUGIN_NAME}.dll
    ${PROJNAME}
    )
