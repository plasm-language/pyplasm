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

Test pyplasm:

```
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```

## Developers only


```
./scripts/new-tag.sh
```

Tests do perform:

```
[OK] resources\examples\arm2d.py
[OK] resources\examples\ashtray.py
[OK] resources\examples\ballorganizer.py
[OK] resources\examples\basic.py
[OK] resources\examples\batch.py
[OK] resources\examples\bsp.py
[OK] resources\examples\colors.py
[OK] resources\examples\graph.py
[OK] resources\examples\lathering.py
[OK] resources\examples\load_obj.py
[OK] resources\examples\pisa.py
[OK] resources\examples\plasm.py
[OK] resources\examples\rotational_solid.py
[OK] resources\examples\temple.py
[OK] resources\examples\threecubes.py

[BROKEN] resources\examples\manipulator.py (but not important, belongs to a different object)
```




