#! /bin/bash

for ntags in 1 10 100 1000 10000
do
    for bogus in 1 2
    do
	python gen_html.py $ntags $bogus > crazy.page
	for bytesize in 1 8192
	do
	    echo $ntags $bogus $bytesize
	    ./tester $bytesize < crazy.page > 1
	    ./pyparser.py < crazy.page > 2
	    diff -i -u 1 2 || exit 1
	done
    done
done
