#!/bin/bash

for i in {3107..3137}
do
../dccp_test -H 192.168.2.14 -P $i -c 100 -l -m 1 -s 480 &
done
