
FIND_PACKAGE(OpenGL REQUIRED)
FILE(GLOB PYPLASM_SOURCES *.h *.hxx *.cpp)

ADD_LIBRARY(xge STATIC ${PYPLASM_SOURCES})

target_include_directories(xge PUBLIC 
	"${OPENGL_INCLUDE_DIR}"
	"${CMAKE_SOURCE_DIR}/libs"
	"${CMAKE_SOURCE_DIR}/libs/zlib"
	"${CMAKE_SOURCE_DIR}/libs/tinyxml"
)
target_link_libraries(xge tinyxml libz qhull Juce ${OPENGL_LIBRARIES})

IF (PYPLASM_WINDOWS)
  TARGET_LINK_LIBRARIES(xge Dbghelp.lib)
elseif (PYPLASM_APPLE)
	TARGET_LINK_LIBRARIES(xge tinyxml "-framework Foundation -framework OpenGL -framework AGL -framework CoreAudio -framework CoreMIDI -framework IOKit -framework QuartzCore -framework AudioToolbox -framework AppKit")
else()
  TARGET_LINK_LIBRARIES(xge dl pthread rt GLU)
ENDIF()


