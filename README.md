# PLASM (Programming LAnguage for Solid Modeling)

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

See also: http://www.plasm.net/

To install pyplasm, if you are using `CPython` distribution:

```
# For Linux sometimes you have to install some python libraries 
# sudo apt-get install python3.6 libpython3/6


python -m pip install --user --upgrade pip
python -m pip install --upgrade GL PyGlut PyOpenGL pyplasm
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```

If you are using `conda` python:

```
conda install  -y --channel scrgiorgio pyplasm
python -c "from pyplasm import *; VIEW(CUBOID([1,1,1]))"
```



<!--//////////////////////////////////////////////////////////////////////// -->
## Commit CI (only for developers

Edit file `src/xgepy/setup.py` and increment the VERSION.

```
TAG=...write the same value of VERSION here...
git commit -a -m "New tag" && git tag -a $TAG -m "$TAG" && git push origin $TAG && git push origin
```


