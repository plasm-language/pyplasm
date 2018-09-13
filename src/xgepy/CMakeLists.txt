
# huge file is generated!
IF (PYPLASM_WINDOWS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF()


INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${NUMPY_INCLUDE_DIRS})


# use SWIG to generate file or not
SET(PYPLASM_REGENERATE_SWIG_WRAPPERS OFF CACHE BOOL "Regenerate swig wrappers")
IF (PYPLASM_REGENERATE_SWIG_WRAPPERS)
  MESSAGE("-- Regenerating swig wrappers")
  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})
  SET(CMAKE_SWIG_FLAGS "")
  SET_SOURCE_FILES_PROPERTIES(xge.i PROPERTIES CPLUSPLUS ON)
  # important -extranative (which define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS see http://old.nabble.com/Re%3A-std_vector-and-typemaps-p32605911.html)  
  SET_SOURCE_FILES_PROPERTIES(xge.i PROPERTIES SWIG_FLAGS "-threads;-extranative")
  SWIG_ADD_MODULE(xgepy python xge.i)
  ADD_DEPENDENCIES(_xgepy xge)
  SWIG_LINK_LIBRARIES(xgepy ${PYTHON_LIBRARIES} xge)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/xgepy.py DESTINATION "${CMAKE_INSTALL_PREFIX}")
ELSE()

  MESSAGE("-- Using existing swig wrappers")
  ADD_LIBRARY(_xgepy SHARED xge.i xgePYTHON_wrap.cxx xgePYTHON_wrap.h xgepy.py)
  
  set(PYTHON_IS_ANACONDA 0)
  string(FIND "${PYTHON_EXECUTABLE}" "anaconda" _index_)
	if("${_index_}" GREATER -1)
  	set(PYTHON_IS_ANACONDA 1)
  endif()
  
  # see https://groups.google.com/a/continuum.io/forum/#!topic/anaconda/057P4uNWyCU
  if(APPLE)
    TARGET_LINK_LIBRARIES(_xgepy xge)
    set_target_properties(_xgepy PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	else()
		TARGET_LINK_LIBRARIES(_xgepy ${PYTHON_LIBRARIES} xge)
	endif()
  
  ADD_DEPENDENCIES(_xgepy xge)
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xgepy.py DESTINATION "${CMAKE_INSTALL_PREFIX}")
  IF (PYPLASM_WINDOWS)
    SET_TARGET_PROPERTIES(_xgepy PROPERTIES SUFFIX ".pyd")
  ELSE()
    SET_TARGET_PROPERTIES(_xgepy PROPERTIES PREFIX "")
    SET_TARGET_PROPERTIES(_xgepy PROPERTIES SUFFIX ".so")
  ENDIF()
ENDIF()

# install the shared library
INSTALL(TARGETS _xgepy DESTINATION ${CMAKE_INSTALL_PREFIX})

# install other *.py files
INSTALL(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/fenvs.py 
  ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py 
  ${CMAKE_CURRENT_SOURCE_DIR}/viewer.py
  ${CMAKE_CURRENT_SOURCE_DIR}/setup.py 
  DESTINATION "${CMAKE_INSTALL_PREFIX}")


# install examples
FILE(GLOB XGEPY_EXAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/examples/*")
INSTALL(FILES ${XGEPY_EXAMPLES} DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/")

 

