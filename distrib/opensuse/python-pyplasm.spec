#
# spec file for package python-pyplasm
#
# Copyright (c) 2013 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

%define py_inc_dir  /usr/include/python2.7
%define py_site_dir /usr/lib/python2.7/site-packages

Name:           python-pyplasm
Version:	1.0_git20130224
Release:	1
License:	LGPL-2.1
Summary:	A design language for gemometric and solid parametric modelling
Url:		https://github.com/plasm-language/pyplasm
Group:		Productivity/Graphics/CAD
# Source:		http://www.plasm.net/download/plasm_314_2/pyplasmsources.zip
## snapshot from github
Source:		master.zip
Source2:	oddshape.py
Patch1:		destdir.patch
Patch2:		disable_gl_check.patch
BuildRequires:	freeimage-devel
BuildRequires:	freetype2-devel
BuildRequires:	alsa-devel
BuildRequires:	glew-devel 
BuildRequires:	fdupes
%if 0%{?suse_version} >= 1230
BuildRequires:	glu-devel
%endif
# for juce:
%if 0%{?suse_version} >= 1220
BuildRequires:	libXinerama-devel
BuildRequires:	libXcursor-devel
%endif
# Install python2.7 (the version number is important): (they say, I hope not -- jw)
BuildRequires:	python-devel 
BuildRequires:	gcc-c++ unzip
Requires:	alsa
Requires:	glew
# W: explicit-lib-dependency
# Requires:	libfreeimage3
# Requires:	libfreetype6
# Requires:	libGLEW1_6 

BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Plasm is a 'design language' for geometric and solid parametric design,
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in
Italy.


%prep
%setup -q -n pyplasm.sources -n pyplasm-master
# destdir.patch
%patch1 -p1
# disable_gl_check.patch
%patch2 -p1

echo PYTHON_INCLUDE_DIRECTORY=%py_inc_dir >> Makefile.inc
echo PYTHON_SITE_PACKAGES=%py_site_dir >> Makefile.inc

%build
make %{?_smp_mflags}

%install
%make_install
chmod -R o-w $RPM_BUILD_ROOT/*
install -d $RPM_BUILD_ROOT%{_libdir}
install src/xge/libxge.so $RPM_BUILD_ROOT%{_libdir}
install %{SOURCE2}        $RPM_BUILD_ROOT%{py_site_dir}/examples

# python-pyplasm.i586: E: files-duplicated-waste (Badness: 100) 8628025
# Tell rpm to do the obviuous.
%fdupes -s $RPM_BUILD_ROOT/

# Tell rpm to do the obviuous. Again.
%post   -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc LICENSE.txt README.rst
# Tell rpm to do the obviuous. The files are where we put them.
/usr/lib/python*
%{_libdir}/lib*

%changelog

