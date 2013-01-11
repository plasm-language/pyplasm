
ADD_DEFINITIONS(-DPYPLASM_BUILDING_XGE=1)

INCLUDE_DIRECTORIES("../../libs")
INCLUDE_DIRECTORIES("../../libs/zlib")
INCLUDE_DIRECTORIES("../../libs/glew")
INCLUDE_DIRECTORIES("../../libs/tinyxml")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibJPEG")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibMNG")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibOpenJPEG")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibPNG/")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibRawLite")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/LibTIFF")
INCLUDE_DIRECTORIES("../../libs/FreeImage${FREEIMAGE_VERSION}/Source/OpenEXR")

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
ADD_DEPENDENCIES(xge FreeImage)
ADD_DEPENDENCIES(xge libz)
ADD_DEPENDENCIES(xge qhull)
ADD_DEPENDENCIES(xge Juce)
TARGET_LINK_LIBRARIES(xge tinyxml FreeImage libz qhull Juce)

if (PYPLASM_LINUX)
  TARGET_LINK_LIBRARIES(xge tinyxml dl pthread rt )
ELSEIF(PYPLASM_APPLE)
  TARGET_LINK_LIBRARIES(xge tinyxml "-framework Foundation")
ENDIF()


