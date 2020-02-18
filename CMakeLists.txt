
# use a recent version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8) 

PROJECT(PyPlasm)

SET(CMAKE_BUILD_TYPE "Release")

# some basic tests 
INCLUDE(CMakeBackwardCompatibilityCXX)
INCLUDE(CheckCSourceCompiles)

# add the possibility to add groups to solution
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS On)

# parallel building
SET (CMAKE_NUM_PROCS 8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

SET(PYTHON_REQUIRED_VERSION 3)

####################################################
# platform
####################################################
IF (WIN32)

  SET(PYPLASM_WINDOWS 1)
  ADD_DEFINITIONS(-DPYPLASM_WINDOWS=1 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN=1)
  MESSAGE("-- Setting PYPLASM_WINDOWS")
  
ELSEIF (APPLE)

  SET(PYPLASM_APPLE 1)
  ADD_DEFINITIONS(-DPYPLASM_APPLE=1)
  
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral -Wformat-security -Wno-write-strings -Wno-reorder -Wno-unused-variable")
  set(CMAKE_MACOSX_RPATH ON)
  MESSAGE("-- Setting PYPLASM_APPLE")
  
ELSE()

  MESSAGE("-- Setting PYPLASM_LINUX")
  SET(PYPLASM_LINUX 1)
  ADD_DEFINITIONS(-DPYPLASM_LINUX=1)
  
  if(NOT CMAKE_BUILD_TYPE)
    MESSAGE(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
  endif()
  
   IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG=1)
  ENDIF() 
  
   # enable 64 bit file support (see http://learn-from-the-guru.blogspot.it/2008/02/large-file-support-in-linux-for-cc.html)
  ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
  SET(CMAKE_C_FLAGS    ${CMAKE_C_FLAGS}   "-fPIC")
  SET(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-fPIC -std=gnu++0x")
  INCLUDE_DIRECTORIES("/usr/local/include")
  INCLUDE_DIRECTORIES("/usr/include")

ENDIF()

ADD_DEFINITIONS(-DPYPLASM_JUCE=1)


FIND_PACKAGE(PythonInterp ${PYTHON_REQUIRED_VERSION} REQUIRED) 
FIND_PACKAGE(PythonLibs   ${PYTHON_REQUIRED_VERSION} REQUIRED)
FIND_PACKAGE(NumPy                                   REQUIRED)

# find site package dir
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} 
	-c  "import site;print(site.getsitepackages()[-1])"  
	OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

MESSAGE("-- PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
message("-- PYTHON_SITE_PACKAGES_DIR ${PYTHON_SITE_PACKAGES_DIR}")


IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PYTHON_SITE_PACKAGES_DIR}/pyplasm" CACHE PATH "" FORCE)
endif()

if (WIN32)
	string(REPLACE "\\" "/" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

MESSAGE("-- CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

ADD_SUBDIRECTORY(libs)
ADD_SUBDIRECTORY(src)

# /////////////////////////////////////////////////////////////////////
# dist target
if (1)
	set(PYTHON_TAG cp${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})

	if (WIN32)
	  set(PLAT_NAME win_amd64)
	elseif(APPLE)

		EXECUTE_PROCESS(COMMAND sw_vers -productVersion OUTPUT_VARIABLE APPLE_OSX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		STRING(REGEX REPLACE "^([0-9]+\\.[0-9]+).*$" "\\1" APPLE_OSX_VERSION "${APPLE_OSX_VERSION}")
		message(STATUS "APPLE_OSX_VERSION ${APPLE_OSX_VERSION}")
	  string(SUBSTRING ${APPLE_OSX_VERSION} 0 2 APPLE_OSX_VERSION_MAJOR)
	  string(SUBSTRING ${APPLE_OSX_VERSION} 3 2 APPLE_OSX_VERSION_MINOR)
	  set(PLAT_NAME macosx_${APPLE_OSX_VERSION_MAJOR}_${APPLE_OSX_VERSION_MINOR}_x86_64)
	else()
	  set(PLAT_NAME linux_x86_64)
	endif()

	add_custom_target(dist 
		${PYTHON_EXECUTABLE} setup.py bdist_wheel --python-tag=${PYTHON_TAG} --plat-name=${PLAT_NAME} 
		WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})
endif()






