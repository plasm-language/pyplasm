
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

ADD_SUBDIRECTORY(libs)
ADD_SUBDIRECTORY(src)





