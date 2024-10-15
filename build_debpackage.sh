#!/bin/bash

if [ -f /usr/lib/`uname -a`-linux-gnu/cmake/Qt6 -a -z "$QT_ROOT" ]; then
    QT_ROOT=/usr/lib/`uname -a`-linux-gnu/cmake/Qt6
fi

if [ -n "$1" ]; then
	QT_ROOT=$1
fi

if [ -z "$QT_ROOT" ]; then
	echo "$0 QT_ROOT"
    exit -1
fi

export QT_ROOT=$QT_ROOT

#fakeroot debian/rules binary

# -p, --sign-command=sign-command
#  When dpkg-buildpackage needs to execute GPG to sign a source
#  control (.dsc) file or a .changes file it will run sign-command
#  (searching the PATH if necessary) instead of gpg (long option since
#  dpkg 1.18.8).  sign-command will get all the arguments that gpg
#  would have gotten. sign-command should not contain spaces or any
#  other shell metacharacters.

# -k, --sign-key=key-id
#  Specify a key-ID to use when signing packages (long option since
#  dpkg 1.18.8).

# -us, --unsigned-source
#  Do not sign the source package (long option since dpkg 1.18.8).

# -ui, --unsigned-buildinfo
#  Do not sign the .buildinfo file (since dpkg 1.18.19).

# -uc, --unsigned-changes
#  Do not sign the .buildinfo and .changes files (long option since
#  dpkg 1.18.8).

# -b  Equivalent to --build=binary or --build=any,all.
# -S  Equivalent to --build=source

#The -us -uc tell it there is no need to GPG sign the package. the -b is build binary
dpkg-buildpackage -us -uc -b

#The -us -uc tell it there is no need to GPG sign the package. the -S is build source package
#dpkg-buildpackage -us -uc -S  

# See: https://mentors.debian.net/intro-maintainers/
#dpkg-buildpackage -S

# build source and binary package
#dpkg-buildpackage -us -uc 

#dpkg-buildpackage
