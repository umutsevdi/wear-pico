#!/bin/bash
#******************************************************************************
#
# * File: lib_gen.sh
#
# * Author:  Umut Sevdi
# * Created: 10/21/23
# * Description: Generate all library files with appropriate namespace from
# Pico SDK and it's auto-generated files
#*****************************************************************************
rm -rf src/.inc
mkdir ./.inc/ -p 2>/dev/null
find pico-sdk/src -type f |  grep "include/.*\.h" | sed -e "s/\(pico-sdk\).*\(include\)\(.*\.h\)/cp \0 .inc\3/g" > f
find build/generated/ -type f | grep "generated/.*\.h" | sed "s/\(build\/generated\)\(\/[_A-Za-z0-9]*\)\(.*\.h\)/cp \0 .inc\3 /g" >> f
find pico-sdk/lib -type f | grep "pico-sdk/lib/[A-Za-z\-_0-9]*/src/.*\.h" | grep -v "example" | grep -v "test" | sed "s/pico-sdk\/lib\/\([A-Za-z\-_0-9]*\)\/src\/\(.*\)/cp \0 .inc\/\2 /g" >> f
# find src/ -type f |  grep "include/.*\.h" | sed -e "s/\(src\).*\(include\)\(.*\.h\)/cp \0 .inc\3/g" >> f


mkdir -p $(awk '{print $3}' f | xargs dirname | sort -u)
bash f
mv .inc src/
rm f
