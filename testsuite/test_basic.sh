#! /bin/bash

for i in `find tstpages -type f | sort`
do
    echo $i
    ./tester 1 < $i > 1
    ./pyparser.py $i > 2
    diff -a -i -u 1 2 || exit 1
done

