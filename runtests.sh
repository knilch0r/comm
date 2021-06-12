#!/bin/sh
set -e
cd tests
FAIL=0
for f in "a b" "b a" ; do
	for i in -12 -23 -13 ; do
		t=`echo $f | tr -d ' '`
		../comm $i $f > testout-mine-$t$i
		comm $i $f > testout-orig-$t$i
		if diff -u testout-orig-$t$i testout-mine-$t$i ; then
			echo test-$t$i: PASSED
			rm testout-orig-$t$i testout-mine-$t$i
		else
			echo test-$t$i: FAILED
			FAIL=1
		fi
	done
done
exit $FAIL

