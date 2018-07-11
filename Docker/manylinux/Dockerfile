FROM quay.io/pypa/manylinux1_x86_64

# /////////////////////////////////////////////

# i need curl to download files
RUN yum install -y curl 

# recent version of cmake
RUN  set -x \
	&& mkdir -p /tmp && cd /tmp \
	&& curl https://cmake.org/files/v3.12/cmake-3.12.0-rc1.tar.gz -O \
	&& tar xvzf cmake-3.12.0-rc1.tar.gz && cd cmake-3.12.0-rc1 \
	&& ./bootstrap \
	&& make -j 8  \
	&& make install  \
	&& hash -r  \
	&& cd .. && rm -Rf cmake-3.12.0-rc1*
	
# openssl 
RUN set -x \
	&& mkdir -p /tmp && cd /tmp \
	&& curl https://www.openssl.org/source/openssl-1.0.2a.tar.gz -O \
	&& tar xvzf openssl-1.0.2a.tar.gz && cd openssl-1.0.2a \
	&& ./config -fpic shared --prefix=/usr/local \
	&& make \
	&& make install \
	&& cd ../ && rm -Rf openssl-1.0.2a* 

# python3
RUN set -x \
	&& mkdir -p /tmp && cd /tmp \
	&& yum install -y zlib-devel \
	&& curl "https://www.python.org/ftp/python/3.6.5/Python-3.6.5.tgz" -O \
	&& tar xvzf Python-3.6.5.tgz \
	&& cd Python-3.6.5  \
	&& echo "SSL=/usr/local/ssl" >> Modules/Setup.dist \
	&& echo "_ssl _ssl.c -DUSE_SSL -I/usr/local/include -I/usr/local/include/openssl -L/usr/local/lib -lssl -lcrypto" >> Modules/Setup.dist \
	&& ./configure -enable-shared \
	&& make -j 4 \
	&& make install \
	&& cd ../ \
	&& rm -Rf Python-3.6.5*
	
# install pip3 packages
RUN set -x \
	&& pip3 install --upgrade pip \
	&& pip3 install numpy setuptools wheel twine auditwheel PyOpenGL PyOpenGL-accelerate


# /////////////////////////////////////////////
# pyplasm (forcing clone)

RUN yum install -y alsa-lib-devel freetype-devel libX11-devel libXext-devel libXinerama-devel libXcursor-devel 
RUN yum install -y libXdamage-devel libXv-devel libxkbfile-devel alsa-lib-devel cups-devel ffmpeg-devel libXrandr-devel


ENV PYPLASM_HOME  /home/pyplasm
ENV GIT_BRANCH master
ENV GITHUB_RAW_URL https://raw.githubusercontent.com/plasm-language/pyplasm/$GIT_BRANCH
ADD https://api.github.com/repos/plasm-language/pyplasm/git/refs/heads/$GIT_BRANCH version.json
RUN git clone -b$GIT_BRANCH https://github.com/plasm-language/pyplasm.git $PYPLASM_HOME
RUN mkdir -p $PYPLASM_HOME/build
WORKDIR $PYPLASM_HOME/build 

...broken from here....

RUN cmake ../
RUN cmake --build . --target all -- -j 4
RUN cmake --build . --target install

WORKDIR /usr/lib/python3.6/dist-packages/pyplasm

RUN ldd ./bin/libxgepy.so # informative
RUN cp /usr/local/lib64/libcrypto* /usr/local/lib64/libssl.so* ./bin
RUN for it in $(find bin/*); do patchelf --set-rpath '$ORIGIN' $it; patchelf --shrink-rpath $it; done
RUN python3 setup.py bdist_wheel --python-tag=cp36 --plat-name=linux_x86_64
	
RUN set -x \	
	&& WHEEL=$(find ./dist -iname "*.whl") \
	&& WHEEL=${WHEEL/linux_x86_64/manylinux1_x86_64} \
	&& mv dist/*.whl $WHEEL \
	&& auditwheel show $WHEEL
	
RUN ldd ./bin/libxgepy.so # informative








  
