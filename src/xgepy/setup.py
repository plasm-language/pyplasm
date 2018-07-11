import os, sys, setuptools
import shutil

#increase this number for PIP
VERSION="1.1.0"
this_dir="."

# findFilesInCurrentDirectory
def findFilesInCurrentDirectory():
	ret=[]
	for dirpath, __dirnames__, filenames in os.walk(this_dir):
	  for filename in filenames:
	    file= os.path.abspath(os.path.join(dirpath, filename))
	    first_dir=dirpath.replace("\\","/").split("/")
	    first_dir=first_dir[1] if len(first_dir)>=2 else ""
	    if first_dir in ("build","dist","pyplasm.egg-info","__pycache__") or file.endswith(".pdb"):
	    	continue
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
