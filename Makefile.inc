
# platform name
PLATFORM_NAME=PLATFORM_$(shell uname)

# current gcc optimization
OPTIMIZATION=-O3


# --- LINUX ---
ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
PYTHON_INCLUDE_DIRECTORY=/usr/include/python2.6
PYTHON_LIB_DIRECTORY=
PYTHON_SITE_PACKAGES=/usr/lib/python2.6/dist-packages
endif

# ---- DARWIN ----
ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
PYTHON_INCLUDE_DIRECTORY=/Library/Frameworks/Python.framework/Versions/2.6/include/python2.6
PYTHON_LIB_DIRECTORY=-L/Library/Frameworks/Python.framework/Versions/2.6/lib/python2.6/config/
PYTHON_SITE_PACKAGES=/Library/Frameworks/Python.framework/Versions/2.6/lib/python2.6/site-packages
endif

# C++ compilation flags
CXXFLAGS=-w -fPIC ${OPTIMIZATION} -D${PLATFORM_NAME} 










