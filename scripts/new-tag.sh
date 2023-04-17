#!/bin/bash

set -ex

TAG=$(python3 src/xgepy/setup.py new-tag) && echo ${TAG}

git commit -a -m "New tag ($TAG)" 
git tag -a $TAG -m "$TAG"
git push origin $TAG
git push origin

