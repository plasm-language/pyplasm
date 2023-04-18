find_package(SWIG 3.0 REQUIRED)
include(${SWIG_USE_FILE})	


set(SwigFile ${CMAKE_CURRENT_SOURCE_DIR}/xge.i)

set_property(SOURCE ${SwigFile} APPEND PROPERTY SWIG_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}")

# this is for generated C++ and header files
set(SWIG_OUTFILE_DIR ${CMAKE_BINARY_DIR}/${IntConfigName}) 

set(CMAKE_SWIG_OUTDIR ${SWIG_OUTFILE_DIR}/pyplasm) # this is for *.py generated files
set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)

set_property(SOURCE ${SwigFile} APPEND PROPERTY SWIG_FLAGS "-threads;-extranative")

set_source_files_properties(${SwigFile} PROPERTIES CPLUSPLUS ON)

file(GLOB PyFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.py)

swig_add_library(xgepy LANGUAGE python SOURCES ${SwigFile} ${PyFiles})
target_compile_definitions(xgepy PRIVATE SWIG_TYPE_TABLE=pyplasm)
target_compile_definitions(xgepy PRIVATE SWIG_PYTHON_INTERPRETER_NO_DEBUG=1)
target_include_directories(xgepy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xgepy PRIVATE xge)
set_target_properties(xgepy PROPERTIES FOLDER Swig/)

target_compile_definitions(xgepy PUBLIC PYPLASM_PYTHON=1)
if (WIN32)
	target_include_directories(xgepy PUBLIC ${Python_INCLUDE_DIRS})
	target_link_libraries(xgepy      PUBLIC ${Python_LIBRARY_RELEASE})
else()
	target_link_libraries(xgepy PUBLIC Python::Module) # Python library for Python extending
endif()

if (MSVC)
	target_compile_definitions(xgepy PRIVATE /W0) 
	target_compile_options(xgepy     PRIVATE /bigobj)
else()
	set_target_properties(xgepy PROPERTIES COMPILE_FLAGS "${BUILD_FLAGS} -w")
endif()

# swig generated *.py file and *.so must be in the same pyplasm/ root directoryc otherwise it won't work
set_target_properties(xgepy PROPERTIES 
	LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm
	RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm
	ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${ConfigName}/swig) 

install(FILES ${PyFiles}         DESTINATION ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm/)
install(DIRECTORY ../../examples DESTINATION ${CMAKE_BINARY_DIR}/${ConfigName}/pyplasm/)

 
