
# platform name
PLATFORM_NAME=$(shell uname)

# could be changed to -O3?
OPTIMIZATION=-O2

# Python headers (make sure it is version 2.6!)
PYTHON_LIBRARY=/Library/Frameworks/Python.framework/Versions/2.6

# C++ compilation flags
CXXFLAGS=-w -fPIC ${OPTIMIZATION} -D${PLATFORM_NAME} -I${PYTHON_LIBRARY}/include/python2.6 

# C++ linking common libraries 
LIBS=-framework CoreFoundation -framework OpenGL -framework Python  











