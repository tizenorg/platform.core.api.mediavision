Name:        capi-media-vision
Summary:     Media Vision library for Tizen Native API
Version:     0.3.18
Release:     0
Group:       Multimedia/Framework
License:     Apache-2.0 and BSD-2.0
Source0:     %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: pkgconfig(capi-media-tool)
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
BuildRequires: pkgconfig(libavcodec)
BuildRequires: pkgconfig(libavformat)
BuildRequires: pkgconfig(libswscale)
BuildRequires: libavutil-devel
BuildRequires: pkgconfig(gstreamer-1.0)
BuildRequires: pkgconfig(gstreamer-base-1.0)
BuildRequires: pkgconfig(gstreamer-app-1.0)
BuildRequires: pkgconfig(libtzplatform-config)

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

%ifarch %{arm}
export CFLAGS="$CFLAGS -DENABLE_NEON"
export CXXFLAGS="$CXXFLAGS -DENABLE_NEON"
%endif

%ifarch %{aarch64}
export CFLAGS="$CFLAGS -DENABLE_NEON"
export CXXFLAGS="$CXXFLAGS -DENABLE_NEON"
%endif

export CFLAGS+=" -DMV_CONFIG_PATH=\\\"%{TZ_SYS_RO_SHARE}/config/%{name}/\\\""
export CXXFLAGS+=" -DMV_CONFIG_PATH=\\\"%{TZ_SYS_RO_SHARE}/config/%{name}/\\\""
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER} -DTZ_SYS_BIN=%TZ_SYS_BIN

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_datadir}/license/
mkdir -p %{buildroot}%{_datadir}/config/%{name}
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/%{name}
cp media-vision-config.json %{buildroot}%{_datadir}/config/%{name}/

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
%{_includedir}/media/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/lib%{name}.so
%TZ_SYS_BIN/*
