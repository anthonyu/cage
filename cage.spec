%define name	cage
%define version	0.80
%define release 1

Summary:	cage - creator of padded cells for untrusted applications
Name:		%{name}
Version:	%{version}
Release:	%{release}
Group:		Utilities/System
Copyright:	GPL
URL:		http://original.killa.net/infosec/cage
Vendor:		Anthony D. Urso <anthony@killa.net>
Source: 	http://original.killa.net/infosec/cage/cage-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-root

%description
cage - creator of padded cells for untrusted applications

%prep
%setup -q -n %{name}-%{version}

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/usr --mandir=/usr/share/man
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root)
%doc COPYING README
/usr/sbin/cage
/usr/share/man/man8/cage.8.gz

%clean 
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%post

%postun

%changelog
* Tue Nov 06 2001 Anthony D. Urso <anthony@killa.net>
- Man page and docs updates.  Source cleansing.
* Wed Oct 02 2001 Anthony D. Urso <anthony@killa.net>
- Man page and documentation updates.
* Thu Sep 20 2001 Anthony D. Urso <anthony@killa.net>
- Corrected missing man page.
* Wed Sep 12 2001 Anthony D. Urso <anthony@killa.net>
- Minor changes to spec.
* Wed Aug 29 2001 Anthony D. Urso <anthony@killa.net>
- Initial spec file created
