To compile Docker:

sudo docker run -it --rm -entrypoint=/bin/bash pyplasm-manylinux
# sudo docker run -it --rm -entrypoint=/bin/bash quay.io/pypa/manylinux1_x86_64

# in case of errors
#sudo docker run --rm -it  <container_id> bash -il

# to upload the wheel
sudo docker run -it --rm -entrypoint=/bin/bash pyplasm-manylinux
twine upload --repository-url https://upload.pypi.org/legacy/ dist/*.whl
        
