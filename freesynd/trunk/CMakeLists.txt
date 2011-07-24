cmake_minimum_required (VERSION 2.6)
project (FreeSynd)

if (WIN32 AND MSVC)
	# Search the bundled libs directory when using MSVC.
	set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
		${CMAKE_SOURCE_DIR}/../../libs/MSVC/lib
	)
	set (CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
		${CMAKE_SOURCE_DIR}/../../libs/MSVC/include
	)
	# Add extra definitions just for MSVC.
	add_definitions (-D_CONSOLE -D_CRT_SECURE_NO_WARNINGS)
	# Override default flags for release builds.
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Oi /Ot /GL")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Ot /GL")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else ()
	set (EXTRA_C_FLAGS "-Wall -Wshadow -Wimplicit -Wno-multichar")
	set (EXTRA_CXX_FLAGS "${EXTRA_C_FLAGS} -Wreorder -Wnon-virtual-dtor")
endif ()

# Set extra compiler flags, possibly different per-platform.
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")

# Set standard definitions for all platforms.
add_definitions (-DSYSTEM_SDL)

# Locate libs we depend on directly.
find_package (SDL)
if (NOT SDL_FOUND)
	message (FATAL_ERROR "Unable to locate SDL")
endif ()

find_package (SDL_image)
if (NOT SDLIMAGE_FOUND)
	message (FATAL_ERROR "Unable to locate SDL_image")
endif ()

find_package (SDL_mixer)
if (NOT SDLMIXER_FOUND)
	message (FATAL_ERROR "Unable to locate SDL_mixer")
endif ()

find_package (PNG)
if (NOT PNG_FOUND)
	message (FATAL_ERROR "Unable to locate PNG")
endif ()

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "debug")
	set (BUILD_DEV_TOOLS TRUE)
	add_definitions (-D_DEBUG)
else ()
	set (BUILD_DEV_TOOLS FALSE)
endif ()

# Make sure we can include from the libs we found.
include_directories (${PNG_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR})

# Build the sources in this subdirectory.
subdirs (src)

