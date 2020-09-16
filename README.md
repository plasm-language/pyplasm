# PLASM (Programming LAnguage for Solid Modeling)

![GitHub Actions](https://github.com/sci-visus/pyplasm/workflows/BuildPyPlasm/badge.svg)

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

See also: http://www.plasm.net/

To install pyplasm, if you are using `CPython` distribution:

```
# For Linux sometimes you have to install some python libraries 
# sudo apt-get install python3.6 libpython3/6


python -m pip install --user --upgrade pip
python -m pip install --upgrade pyplasm
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```

If you are using `conda` python:

```
conda install  -y --channel pyplasm pyplasm
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```



