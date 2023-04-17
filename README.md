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
python -m pip install numpy PyGlut PyOpenGL 
```

on other platforms:

```
source .venv/bin/activate
python -m pip install numpy PyGlut PyOpenGL 
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

Create a conda environment (change python version as needed):

```
conda create -y -n my-env -c conda-forge python=3.9 numpy pyopengl freeglut 
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
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```

Other misc tests:

```
python resources/examples/arm2d.py
python resources/examples/ashtray.py
python resources/examples/ballorganizer.py
python resources/examples/basic.py
python resources/examples/batch.py
python resources/examples/bsp.py
python resources/examples/colors.py
python resources/examples/graph.py
python resources/examples/lathering.py
python resources/examples/load_obj.py
python resources/examples/pisa.py
python resources/examples/plasm.py
python resources/examples/rotational_solid.py
python resources/examples/temple.py
python resources/examples/threecubes.py
```




