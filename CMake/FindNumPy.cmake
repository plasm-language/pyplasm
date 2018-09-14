# Find the Python NumPy package

execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c "from __future__ import print_function\ntry: import numpy; print(numpy.get_include(), end='')\nexcept:pass\n" OUTPUT_VARIABLE __numpy_path)
find_path(NUMPY_INCLUDE_DIR numpy/arrayobject.h HINTS "${__numpy_path}" "${PYTHON_INCLUDE_PATH}" NO_DEFAULT_PATH)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NumPy REQUIRED_VARS NUMPY_INCLUDE_DIR)
                                        