import os,sys,shutil,setuptools

#increase this number for PIP/conda
VERSION="2.0.9"
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

is_conda = os.path.exists(os.path.join(sys.prefix, 'conda-meta', 'history'))
if is_conda:
	from distutils.core import setup, Extension
	import distutils.command.bdist_conda
	setup(
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
		distclass=distutils.command.bdist_conda.CondaDistribution,
		conda_buildnum=1)
	
else:
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
		license = "GPL")
