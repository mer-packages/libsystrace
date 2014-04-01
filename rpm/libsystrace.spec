Name: libsystrace
Version: 0.0.0
Release: 1
Summary: A library for logging systrace data.
Group: System/Libraries
License: BSD
URL: https://github.com/mer-packages/
Source0: %{name}-%{version}.tar.bz2
BuildRequires: qt5-qmake
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
%summary.

%package devel
Summary: Development files for %{name}
Group: System/Libraries

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_libdir}/libsystrace.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/libsystrace.so
%{_libdir}/pkgconfig/systrace.pc
%{_includedir}/systrace.h

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
