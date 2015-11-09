Name:        capi-media-vision
Summary:     Media Vision library for Tizen Native API
Version:     0.2.5
Release:     0
Group:       Multimedia/Framework
License:     Apache-2.0 and BSD-2.0
Source0:     %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: pkgconfig(capi-media-tool)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(libtbm)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(opencv)
BuildRequires: pkgconfig(zbar)
BuildRequires: pkgconfig(glib-2.0)
# Change to the pkgconfig(zint) after zint package refactor
BuildRequires: zint
BuildRequires: zint-devel
BuildRequires: pkgconfig(json-glib-1.0)
BuildRequires: dlogutil
BuildRequires: libjpeg-turbo
BuildRequires: libjpeg-turbo-devel
BuildRequires: pkgconfig(libavcodec)
BuildRequires: pkgconfig(libavformat)
BuildRequires: pkgconfig(libswscale)
BuildRequires: libavutil-devel
BuildRequires: pkgconfig(gstreamer-1.0)
BuildRequires: pkgconfig(gstreamer-base-1.0)
BuildRequires: pkgconfig(gstreamer-app-1.0)

%description
Media Vision library for Tizen Native API. Includes barcode detecting, barcode generating, face and image modules.

%package devel
Summary:    Multimedia Vision Library (DEV)
Group:      Multimedia/Framework
Requires:   %{name} = %{version}-%{release}

%description devel
Media Vision library for Tizen Native API (DEV). Includes barcode detecting, barcode generating, face and image modules.

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license/
mkdir -p %{buildroot}/usr/share/config/%{name}
cp LICENSE.APLv2 %{buildroot}/usr/share/license/%{name}
cp media-vision-config.json %{buildroot}/usr/share/config/%{name}/

%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%manifest capi-media-vision.manifest
%{_datadir}/license/%{name}
%{_datadir}/config/%{name}/media-vision-config.json
%{_libdir}/libcapi-media-vision.so.*
%{_libdir}/libmv*.so

%files devel
#%{_datadir}/config/%{name}/media-vision-config.json
%{_includedir}/media/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/lib%{name}.so
/opt/usr/devel/media/testsuites/*
