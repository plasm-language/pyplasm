CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# swig related 
if (POLICY CMP0086)
  cmake_policy(SET CMP0086 OLD) 
endif()

# swig related
if (POLICY CMP0078)
  cmake_policy(SET CMP0078 OLD)  
endif()

# cmake generator expression in install
if (POLICY CMP0087)
  cmake_policy(SET CMP0087 NEW)  
endif()

# use cmake folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  

# disable incremental linking for windows
SET(MSVC_INCREMENTAL_DEFAULT OFF) 

# independent code position
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

PROJECT(PyPlasm)

# detect compiler
if (MSVC)
	SET(PYPLASM_WINDOWS 1)
	ADD_DEFINITIONS(-DPYPLASM_WINDOWS=1 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN=1)
	MESSAGE(STATUS "Detected visual studio (C++ __MSVC_VER - WIN32)")

elseif (CMAKE_CXX_COMPILER MATCHES ".*clang")
	set(CLANG 1)
	SET(PYPLASM_APPLE 1)
	ADD_DEFINITIONS(-DPYPLASM_APPLE=1)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral -Wformat-security -Wno-write-strings -Wno-reorder -Wno-unused-variable")
	set(CMAKE_MACOSX_BUNDLE TRUE)
	set(CMAKE_MACOSX_RPATH  TRUE)
	MESSAGE(STATUS "Detected apple clang (C++: __clang__ __APPLE__)")

else()
	set(GNUC  1)
	SET(PYPLASM_LINUX 1)
	ADD_DEFINITIONS(-DPYPLASM_LINUX=1)
	# enable 64 bit file support (see http://learn-from-the-guru.blogspot.it/2008/02/large-file-support-in-linux-for-cc.html)
	ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -Wno-write-strings)
	SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-std=gnu++0x")
	INCLUDE_DIRECTORIES("/usr/local/include")
	INCLUDE_DIRECTORIES("/usr/include")
	MESSAGE(STATUS "Detected GNU ")

endif()

if (CMAKE_CONFIGURATION_TYPES)
	set(ConfigName         $<CONFIG>)
	set(IntConfigName      ${CMAKE_CFG_INTDIR})
else()
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
		set(CMAKE_BUILD_TYPE "Release")
	endif()
	set(ConfigName         ${CMAKE_BUILD_TYPE})
	set(IntConfigName      ${CMAKE_BUILD_TYPE})
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ConfigName}/lib)
set(CMAKE_INSTALL_PREFIX           ${CMAKE_BINARY_DIR} CACHE STRING "CMAKE_INSTALL_PREFIX" FORCE)

include(FindPackageHandleStandardArgs)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
MESSAGE(STATUS "Python_EXECUTABLE ${Python_EXECUTABLE}")
MESSAGE(STATUS "Python_VERSION    ${Python_VERSION}")

ADD_SUBDIRECTORY(libs)
ADD_SUBDIRECTORY(src)