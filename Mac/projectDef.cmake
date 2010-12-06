#/**********************************************************\ 
# Auto-generated Mac project definition file for the
# WebP2P project
#\**********************************************************/

# Mac template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Mac/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.cpp
    Mac/[^.]*.h
    Mac/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    
)


SOURCE_GROUP(Mac FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

set(PLIST "Mac/bundle_template/Info.plist")
set(STRINGS "Mac/bundle_template/InfoPlist.strings")
set(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJNAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)

find_path(PJ_INCLUDES pjlib.h /usr/local/include)
if(NOT PJ_INCLUDES)
	message(FATAL_ERROR "Could not find pjlib.h")
endif(NOT PJ_INCLUDES)
include_directories(BEFORE ${PJ_INCLUDES})

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
find_library(PJ_LIB pj-i386-x86_64-apple-darwin10.4.0 /usr/local/lib)
if(NOT PJ_LIB)
  message(FATAL_ERROR "Could not find pj-i386-x86_64-apple-darwin10.4.0")
endif()
find_library(PJ_LIB_UTIL pjlib-util-i386-x86_64-apple-darwin10.4.0 /usr/local/lib)
if(NOT PJ_LIB_UTIL)
  message(FATAL_ERROR "Could not find pjlib-util-i386-x86_64-apple-darwin10.4.0")
endif()
find_library(PJ_NATH pjnath-i386-x86_64-apple-darwin10.4.0 /usr/local/lib)
if(NOT PJ_NATH)
  message(FATAL_ERROR "Could not find pjnath-i386-x86_64-apple-darwin10.4.0")
endif()

target_link_libraries(${PROJNAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${PJ_LIB}
    ${PJ_LIB_UTIL}
    ${PJ_NATH}
    )


