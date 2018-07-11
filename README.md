PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

Table of content:

[Install using PIP binaries](#pip-distribution)

[Windows compilation](#windows-compilation)

[MacOSX compilation](#macosx-compilation)

[Linux compilation](#linux-compilation)


# Pip Distribution


In windows type:

```
c:\Python36\Scripts\pip uninstall pyplasm
c:\Python36\Scripts\pip install pyplasm
c:\Python36\python.exe -c "from pyplasm import *; c=CUBOID([1,1,1]); VIEW(c)"
```

In Linux/MacOSX type:

```
pip3 uninstall pyplasm
pip3 install pyplasm
```


# Windows compilation 

Install [Python 3.x](https://www.python.org/ftp/python/3.6.3/python-3.6.3-amd64.exe) 
You may want to check "*Download debugging symbols*" and "*Download debugging libraries*" if you are planning to debug your code. 

Install numpy and deploy packages:

```
set PYTHON=c:\python36\python.exe
set PIP=c:\Python36\Scripts\pip.exe
%PIP% install PyOpenGL numpy
```

Install git, cmake and swig. The fastest way is to use `chocolatey` i.e from an Administrator Prompt:

```
@"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"
choco install -y  -allow-empty-checksums git cmake swig
```

Instal Visual Studio 2017. You can do:

```
choco install visualstudio2017community
```

Open a DOS prompt and type:

```
cd c:\
mkdir projects
cd projects
git clone https://github.com/plasm-language/pyplasm
cd pyplasm
mkdir build
cd build

set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
%CMAKE% -G "Visual Studio 15 2017 Win64" ..
%CMAKE% --build . --target ALL_BUILD --config Release
%CMAKE% --build . --target INSTALL   --config Release
```

Test if it's working:

```
%PYTHON% -c "from pyplasm import *; c=CUBOID([1,1,1]); VIEW(c)"
```

(OPTIONAL For developers) If you want to upload to PIP
```
cd C:\Python36\lib\site-packages\pyplasm
%PYTHON% setup.py bdist_wheel --python-tag=cp36 --plat-name=win_amd64 
c:\Python36\Scripts\twine.exe upload --repository-url https://upload.pypi.org/legacy/ dist/*.whl
```

# MacOsX compilation 

Install XCode tools from AppStore (optionally install also Xcode command line tools)

Install brew and pyplasm prerequisites:

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install git cmake swig python3
sudo pip3 -H install numpy setuptools wheel twine pyopengl
```

Run xcode command line tools:
```
sudo xcode-select --install
# if command line tools do not work, type the following:
# sudo xcode-select --r
```

From a terminal:

```
git clone git://github.com/plasm-language/pyplasm.git
cd pyplasm
mkdir build && cd build
cmake -G Xcode ../
cmake      --build . --target ALL_BUILD --config Release
sudo cmake --build . --target install   --config Release 
```

Test if it's working:

```
python3 -c "from pyplasm import *; c=CUBOID([1,1,1]); VIEW(c)"
```

# Linux compilation

Install prerequisites:

```
sudo apt-get install python3 python3-dev libasound2-dev libfreetype6-dev xorg-dev libglu1-mesa-dev python3-pip libldap2-dev
```

Install some extra python packages (make sure to use the right pip3 version):

```
sudo pip3 install setuptools PyOpenGL PyOpenGL-accelerate  numpy
```

Generate makefiles and make binaries:

```
git clone git://github.com/plasm-language/pyplasm.git
cd pyplasm
mkdir build && cd build
cmake ../
make -j 4
sudo make install
```

Test if it's working:

```
python3 -c "from pyplasm import *; c=CUBOID([1,1,1]); VIEW(c)"
```

