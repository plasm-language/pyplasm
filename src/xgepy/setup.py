import os,sys,shutil

#increase this number for PIP/conda
PROJECT_VERSION="3.0.10"

this_dir=os.path.dirname(os.path.abspath(__file__))

# ///////////////////////////////////////////////////////////////
def NewTag():
	new_version=PROJECT_VERSION.split(".")
	new_version[2]=str(int(new_version[2])+1)
	new_version=".".join(new_version)
	with open(__file__, 'r') as file : content = file.read()
	content = content.replace('PROJECT_VERSION="{0}"'.format(PROJECT_VERSION), 'PROJECT_VERSION="{0}"'.format(new_version))
	with open(__file__, 'w') as file: file.write(content)		
	print(new_version)

# ///////////////////////////////////////////////////////////////
def DoSetup():

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

	from setuptools import setup
	is_conda = os.path.exists(os.path.join(sys.prefix, 'conda-meta', 'history'))
	if is_conda:
		import conda_build.bdist_conda
		extra_args={"distclass": conda_build.bdist_conda.CondaDistribution,"conda_buildnum": 1}	
	else:
		extra_args={}

	setup(
		name = "pyplasm",
		description = "Plasm is a design language for geometric and solid parametric design, developed by the CAD Group at the Universities La Sapienza and Roma Tre in Italy",
		version=PROJECT_VERSION,
		url="https://github.com/plasm-language/pyplasm",
		author="Giorgio Scorzelli",
		author_email="scorzell@dia.uniroma3.it",
		packages=["pyplasm"],
		package_dir={"pyplasm":'.'},
		package_data={"pyplasm": findFilesInCurrentDirectory()},
		platforms=['Linux', 'OS-X', 'Windows'],
		license = "GPL",
		**extra_args
	)
	
# ///////////////////////////////////////////////////////////////
if __name__ == "__main__":

  if len(sys.argv)>=2 and sys.argv[1]=="print-tag":
    print(PROJECT_VERSION)
    sys.exit(0)
	
  if len(sys.argv)>=2 and sys.argv[1]=="new-tag":
    NewTag()
    sys.exit(0)	

  DoSetup()
  sys.exit(0)	


