
 
# try to disable warnings
SET(CMAKE_C_WARNING_LEVEL   0)
SET(CMAKE_CXX_WARNING_LEVEL 0)
IF(WIN32)
  SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /W0")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
ELSE()
	SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -w")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
ENDIF()

# ZLIB 
ADD_SUBDIRECTORY(zlib)
set_target_properties(libz PROPERTIES FOLDER libs/)

# QHULL 
ADD_SUBDIRECTORY(qhull)
set_target_properties(qhull PROPERTIES FOLDER libs/)

# TINYXML
ADD_SUBDIRECTORY(tinyxml)
set_target_properties(tinyxml PROPERTIES FOLDER libs/)

# JUCE
ADD_SUBDIRECTORY(JUCE)
set_target_properties(Juce PROPERTIES FOLDER libs/)



