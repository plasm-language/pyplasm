
ADD_DEFINITIONS(-DPYPLASM_BUILDING_XGE=1)

FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

INCLUDE_DIRECTORIES("../../libs")
INCLUDE_DIRECTORIES("../../libs/zlib")
INCLUDE_DIRECTORIES("../../libs/tinyxml")

FILE(GLOB PYPLASM_FILES  *.h *.hxx *.cpp )
SOURCE_GROUP("" FILES ${PYPLASM_FILES})
FILE(GLOB PYPLASM_SOURCES ${PYPLASM_FILES})

if (PYPLASM_WINDOWS)
  foreach( src_file ${PYPLASM_SOURCES} )
  	set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS "/Yuxge/xge.h /FIxge/xge.h")
  endforeach()
  set_source_files_properties(xge.cpp PROPERTIES COMPILE_FLAGS "/Ycxge/xge.h")
endif()


ADD_LIBRARY(xge STATIC ${PYPLASM_SOURCES})
ADD_DEPENDENCIES(xge tinyxml)
ADD_DEPENDENCIES(xge libz)
ADD_DEPENDENCIES(xge qhull)
ADD_DEPENDENCIES(xge Juce)
TARGET_LINK_LIBRARIES(xge tinyxml libz qhull Juce ${OPENGL_LIBRARIES})

IF (PYPLASM_WINDOWS)
  TARGET_LINK_LIBRARIES(xge Dbghelp.lib)
ENDIF()

if (PYPLASM_LINUX)
  TARGET_LINK_LIBRARIES(xge dl pthread rt GLU)
ENDIF()

IF (PYPLASM_APPLE)
  TARGET_LINK_LIBRARIES(xge tinyxml "-framework Foundation -framework OpenGL -framework AGL -framework CoreAudio -framework CoreMIDI -framework IOKit -framework QuartzCore -framework AudioToolbox -framework AppKit")
ENDIF()


