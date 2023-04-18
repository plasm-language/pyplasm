# PLASM (Programming LAnguage for Solid Modeling)

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy. See also: http://www.plasm.net/

# cpython

Create a virtual environment:

```
python -m pip install virtualenv
python -m venv .venv
```

Activate the virtual environment. 

On Windows:

```
.\.venv\Scripts\activate.bat
python -m pip install numpy PyOpenGL 
```

on other platforms:

```
source .venv/bin/activate
python -m pip install numpy PyOpenGL 
```

Install pyplasm:

```
python -m pip install --upgrade pyplasm
```

Test pyplasm:

```
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```


# Conda

Create a conda environment:

```
conda create -y -n my-env -c conda-forge python=3.9 numpy pyopengl
```

Activate the environment:

```
conda activate my-env
```

Install pyplasm:

```
conda install -c scrgiorgio -y pyplasm 
``` 

# Test

Simple cuboid:

```
python3 -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```

Other misc tests:

```
python3 examples/arm2d.py
python3 examples/ashtray.py
python3 examples/ballorganizer.py
python3 examples/basic.py
python3 examples/batch.py
python3 examples/bsp.py
python3 examples/colors.py
python3 examples/graph.py
python3 examples/lathering.py
python3 examples/load_obj.py
python3 examples/pisa.py
python3 examples/plasm.py
python3 examples/rotational_solid.py
python3 examples/temple.py
python3 examples/threecubes.py
```

# Developers only

To compile pyplasm, `.github/workflows/build.yml` directory and `resources/scripts`

To push new tag:

```
./resources/scripts/new-tag.sh
```

For `macosx/conda/arm64` you need to compile manually (since GitHub actions does not yet support it):

```:
export ANACONDA_TOKEN=<your-value-here>
export GIT_TAG=<your-value-here>  
for PYTHON_VERSION in 3.8 3.9 3.10 3.11; do
   # internally it will use `uname -m` to detect architecture
   ./resources/scripts/conda.macos.sh
done
```


