FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

# Enable stack-unwinding support in c objects on gcc-based platforms.
# Failing to do so will cause your program to be terminated when a png
# or a jpeg exception is thrown on linux or macosx.
IF (CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
ENDIF(CMAKE_COMPILER_IS_GNUCC)


IF (WIN32)
  SET(JUCE_PLATFORM_SPECIFIC_DIR src/native/windows)
  SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES
    kernel32.lib user32.lib shell32.lib gdi32.lib vfw32.lib comdlg32.lib winmm.lib wininet.lib
    rpcrt4.lib ole32.lib advapi32.lib ws2_32.lib OpenGL32.Lib GlU32.Lib Version.lib Imm32.lib Shlwapi.lib
  )
ELSE()
  IF (UNIX)
    IF (APPLE)
      SET(JUCE_PLATFORM_SPECIFIC_DIR src/native/mac)
      #SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES "-framework Carbon -framework Cocoa -framework CoreFoundation -framework CoreAudio -framework CoreMidi -framework IOKit -framework AGL -lobjc")
    ELSE()
      FIND_PACKAGE(X11 REQUIRED)
      INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
      INCLUDE_DIRECTORIES(/usr/include/freetype2)
      ADD_DEFINITIONS(-DLINUX)
      SET(JUCE_PLATFORM_SPECIFIC_DIR src/native/linux)
      SET(JUCE_PLATFORM_SPECIFIC_LIBRARIES ${X11_LIBRARIES} asound freetype Xinerama dl)
    ENDIF()
  ENDIF()
ENDIF()


# ///////////////////////////////////////////////////////////////////////////////////////////////////////////
# use "find . -type d" inside Juce src directory (then remove src/audio/plugin_* because you do not need them)
# ///////////////////////////////////////////////////////////////////////////////////////////////////////////
SET(SOURCE_DIRECTORIES
  modules/juce_core
  modules/juce_core/containers
  modules/juce_core/files
  modules/juce_core/json
  modules/juce_core/logging
  modules/juce_core/maths
  modules/juce_core/memory
  modules/juce_core/misc
  modules/juce_core/native
  modules/juce_core/native/java
  modules/juce_core/network
  modules/juce_core/streams
  modules/juce_core/system
  modules/juce_core/text
  modules/juce_core/threads
  modules/juce_core/time
  modules/juce_core/unit_tests
  modules/juce_core/xml
  modules/juce_core/zip
  modules/juce_core/zip/zlib
  modules/juce_cryptography
  modules/juce_cryptography/encryption
  modules/juce_cryptography/hashing
  modules/juce_data_structures
  modules/juce_data_structures/app_properties
  modules/juce_data_structures/undomanager
  modules/juce_data_structures/values
  modules/juce_events
  modules/juce_events/broadcasters
  modules/juce_events/interprocess
  modules/juce_events/messages
  modules/juce_events/native
  modules/juce_events/timers
  modules/juce_graphics
  modules/juce_graphics/colour
  modules/juce_graphics/contexts
  modules/juce_graphics/effects
  modules/juce_graphics/fonts
  modules/juce_graphics/geometry
  modules/juce_graphics/images
  modules/juce_graphics/image_formats
  modules/juce_graphics/image_formats/jpglib
  modules/juce_graphics/image_formats/pnglib
  modules/juce_graphics/native
  modules/juce_graphics/placement
  modules/juce_gui_basics
  modules/juce_gui_basics/application
  modules/juce_gui_basics/buttons
  modules/juce_gui_basics/commands
  modules/juce_gui_basics/components
  modules/juce_gui_basics/drawables
  modules/juce_gui_basics/filebrowser
  modules/juce_gui_basics/keyboard
  modules/juce_gui_basics/layout
  modules/juce_gui_basics/lookandfeel
  modules/juce_gui_basics/menus
  modules/juce_gui_basics/misc
  modules/juce_gui_basics/mouse
  modules/juce_gui_basics/native
  modules/juce_gui_basics/positioning
  modules/juce_gui_basics/properties
  modules/juce_gui_basics/widgets
  modules/juce_gui_basics/windows
  modules/juce_gui_extra
  modules/juce_gui_extra/code_editor
  modules/juce_gui_extra/documents
  modules/juce_gui_extra/embedding
  modules/juce_gui_extra/lookandfeel
  modules/juce_gui_extra/misc
  modules/juce_gui_extra/native
  modules/juce_opengl
  modules/juce_opengl/native
  modules/juce_opengl/opengl
)

# /////////////////////////////////////////////////////
macro(ADD_FILES_TO_PROJECT dir)
  if (APPLE)
    FILE(GLOB TEMP ${dir}/*.h ${dir}/*.mm ${dir}/*.cpp)
  ELSE()
    FILE(GLOB TEMP ${dir}/*.h ${dir}/*.cpp)
  ENDIF()
	set_source_files_properties(${TEMP} PROPERTIES HEADER_FILE_ONLY TRUE)
	SET(JUCE_FILES ${JUCE_FILES} ${TEMP})
	STRING(REPLACE "/" "\\" GROUPNAME ${dir})
	SOURCE_GROUP (${GROUPNAME} FILES ${TEMP})
endmacro()

FOREACH (dir ${SOURCE_DIRECTORIES})
	ADD_FILES_TO_PROJECT(${dir})
ENDFOREACH()

SET(JUCE_FILES ${JUCE_FILES} juce.h AppConfig.h)
SOURCE_GROUP ("" FILES juce.h AppConfig.h)


IF (APPLE)
  set_source_files_properties(modules/juce_core/juce_core.mm
    modules/juce_gui_basics/juce_gui_basics.mm
    modules/juce_data_structures/juce_data_structures.mm
    modules/juce_events/juce_events.mm
    modules/juce_graphics/juce_graphics.mm
    modules/juce_opengl/juce_opengl.mm
    modules/juce_cryptography/juce_cryptography.mm
    modules/juce_gui_extra/juce_gui_extra.mm PROPERTIES HEADER_FILE_ONLY FALSE)
ELSE()
  set_source_files_properties(modules/juce_core/juce_core.cpp
    modules/juce_gui_basics/juce_gui_basics.cpp
    modules/juce_data_structures/juce_data_structures.cpp
    modules/juce_events/juce_events.cpp
    modules/juce_graphics/juce_graphics.cpp
    modules/juce_opengl/juce_opengl.cpp
    modules/juce_cryptography/juce_cryptography.cpp
    modules/juce_gui_extra/juce_gui_extra.cpp PROPERTIES HEADER_FILE_ONLY FALSE)
ENDIF()

include_directories(./)
FILE(GLOB JUCE_SOURCES  ${JUCE_FILES})
ADD_LIBRARY(Juce ${JUCE_SOURCES})
TARGET_LINK_LIBRARIES(Juce ${JUCE_PLATFORM_SPECIFIC_LIBRARIES} )

