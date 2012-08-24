# platform name
PLATFORM_NAME=PLATFORM_$(shell uname)

# current gcc optimization
OPTIMIZATION=-O3

# C compilation flags
CFLAGS=-w -fPIC -fno-common

# C++ compilation flags
CXXFLAGS=-w -fPIC ${OPTIMIZATION} -D${PLATFORM_NAME} 

# default link flags
LDFLAGS=-fPIC


# --- windows ---
ifeq ($(PLATFORM_NAME),PLATFORM_CYGWIN_NT-6.1-WOW64)
PYTHON_SITE_PACKAGES=/cygdrive/c/Python26/Lib/site-packages
endif


# --- LINUX ---
ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
    PYTHON_INCLUDE_DIRECTORY=/usr/include/python2.7
    PYTHON_SITE_PACKAGES=/usr/lib/python2.7/dist-packages
endif

# ---- DARWIN ----
ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
PYTHON_INCLUDE_DIRECTORY=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7
PYTHON_SITE_PACKAGES=/Library/Python/2.7/site-packages
CFLAGS+= -arch x86_64  -arch i386
CXXFLAGS+= -arch x86_64 -arch i386
LDFLAGS+= -arch x86_64  -arch i386
endif
















