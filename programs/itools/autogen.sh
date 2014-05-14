#!/bin/sh
#--
# $Id: autogen.sh 10749 2006-10-05 19:33:43Z thamer $
#--

# Handle arguments
if test "$1" = "clean"; then
    echo "Removing auto-generated files..."
    rm -rf configure config.log config.status \
           Makefile config.h
    exit
fi

# Check for missing tools
EXIT=no

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo "Error: You must have \`autoconf' installed."
  EXIT="yes"
}

if test "$EXIT" = "yes"; then
    exit
fi


echo "Running autoconf..."
autoconf configure.in > configure && chmod +x configure
rm -rf autom4te.cache

echo "You can now run \"./configure\" and then \"make\"."
