import os, sys, setuptools
import shutil

#increase this number for PIP
VERSION="1.2.5"
this_dir="."


shutil.rmtree('./build', ignore_errors=True)
shutil.rmtree('./dist', ignore_errors=True)
shutil.rmtree('./pyplasm.egg-info', ignore_errors=True)
shutil.rmtree('./__pycache__', ignore_errors=True)
shutil.rmtree('./examples/__pycache__', ignore_errors=True)

# findFilesInCurrentDirectory
def findFilesInCurrentDirectory():
	ret=[]
	for dirpath, __dirnames__, filenames in os.walk("."):
	  for filename in filenames:
	    file= os.path.abspath(os.path.join(dirpath, filename))
	    ret.append(file)
	return ret

setuptools.setup(
  name = "pyplasm",
  description = "Plasm is a design language for geometric and solid parametric design, developed by the CAD Group at the Universities La Sapienza and Roma Tre in Italy",
  version=VERSION,
  url="https://github.com/plasm-language/pyplasm",
  author="Giorgio Scorzelli",
  author_email="scorzell@dia.uniroma3.it",
  packages=["pyplasm"],
  package_dir={"pyplasm":'.'},
  package_data={"pyplasm": findFilesInCurrentDirectory()},
  platforms=['Linux', 'OS-X', 'Windows'],
  license = "GPL",
  install_requires=['numpy', ],
)
