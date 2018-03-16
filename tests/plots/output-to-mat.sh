#!/bin/bash

transform() {
    cat $1 | awk 'NF>1{a[$2] = a[$2]"\t"$4};END{for(i in a)print a[i]}' | cut -d$'\t' -f2-
}

transform $1
