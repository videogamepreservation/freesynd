#!/bin/bash

base=`basename "$2" .h`
upper=`echo "$base" | tr a-z A-Z`
(
    echo "#ifndef EMBEDDED_${upper}_H"
    echo "#define EMBEDDED_${upper}_H"
    echo "const char embedded_$base[] = "
    cat "$1" | sed -e 's@\\@\\\\@g' -e 's/"/\\"/g' -e 's/^/"/' -e 's/$/\\n"/'
    echo ";"
    echo "#endif"
) > "$2"

