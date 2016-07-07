#!/bin/bash

#when removing a test, remove also the result validation line and vice versa

set +x

echo "run all"

./tnvme --test=0 > test0.txt 2>&1
./tnvme --test=1 > test1.txt 2>&1
./tnvme --test=2 > test2.txt 2>&1
./tnvme --test=3 > test3.txt 2>&1
./tnvme --test=4 > test4.txt 2>&1
./tnvme --test=5 > test5.txt 2>&1
./tnvme --test=6 > test6.txt 2>&1
./tnvme --test=7 > test7.txt 2>&1
./tnvme --test=8 > test8.txt 2>&1
./tnvme --test=9 > test9.txt 2>&1
./tnvme --test=10 > test10.txt 2>&1
./tnvme --test=11 > test11.txt 2>&1
./tnvme --test=12 > test12.txt 2>&1
./tnvme --test=13 > test13.txt 2>&1
./tnvme --test=14 > test14.txt 2>&1
./tnvme --test=15 > test15.txt 2>&1
./tnvme --test=16 > test16.txt 2>&1
./tnvme --test=17 > test17.txt 2>&1
./tnvme --test=18 > test18.txt 2>&1
./tnvme --test=19 > test19.txt 2>&1
./tnvme --test=20 > test20.txt 2>&1
./tnvme --test=21 > test21.txt 2>&1
./tnvme --test=22 > test22.txt 2>&1
./tnvme --test=23 > test23.txt 2>&1
./tnvme --test=24 > test24.txt 2>&1

wait

echo "check sanity"

if [ ! -e test0.txt ] || [ $(grep -c "failed       : 1" test0.txt) -ne 1 ] ||   #one test fails
   [ ! -e test1.txt ] || [ $(grep -c "failed       : 0" test1.txt) -ne 1 ] || 
   [ ! -e test2.txt ] || [ $(grep -c "failed       : 0" test2.txt) -ne 1 ] || 
   [ ! -e test3.txt ] || [ $(grep -c "failed       : 0" test3.txt) -ne 1 ] ||
   [ ! -e test4.txt ] || [ $(grep -c "failed       : 0" test4.txt) -ne 1 ] ||
   [ ! -e test5.txt ] || [ $(grep -c "failed       : 0" test5.txt) -ne 1 ] || 
   [ ! -e test6.txt ] || [ $(grep -c "failed       : 0" test6.txt) -ne 1 ] || 
   [ ! -e test7.txt ] || [ $(grep -c "failed       : 0" test7.txt) -ne 1 ] || 
   [ ! -e test8.txt ] || [ $(grep -c "failed       : 0" test8.txt) -ne 1 ] || 
   [ ! -e test9.txt ] || [ $(grep -c "failed       : 0" test9.txt) -ne 1 ] || 
   [ ! -e test10.txt ] || [ $(grep -c "failed       : 0" test10.txt) -ne 1 ] || 
   [ ! -e test11.txt ] || [ $(grep -c "failed       : 0" test11.txt) -ne 1 ] || 
   [ ! -e test12.txt ] || [ $(grep -c "failed       : 0" test12.txt) -ne 1 ] || 
   [ ! -e test13.txt ] || [ $(grep -c "failed       : 0" test13.txt) -ne 1 ] || 
   [ ! -e test14.txt ] || [ $(grep -c "failed       : 0" test14.txt) -ne 1 ] || 
   [ ! -e test15.txt ] || [ $(grep -c "failed       : 0" test15.txt) -ne 1 ] || 
   [ ! -e test16.txt ] || [ $(grep -c "failed       : 1" test16.txt) -ne 1 ] ||  #one test fails
   [ ! -e test17.txt ] || [ $(grep -c "failed       : 1" test17.txt) -ne 1 ] ||  #one test fails
   [ ! -e test18.txt ] || [ $(grep -c "failed       : 0" test18.txt) -ne 1 ] || 
   [ ! -e test19.txt ] || [ $(grep -c "failed       : 0" test19.txt) -ne 1 ] || 
   [ ! -e test20.txt ] || [ $(grep -c "failed       : 0" test20.txt) -ne 1 ] || 
   [ ! -e test21.txt ] || [ $(grep -c "failed       : 0" test21.txt) -ne 1 ] || 
   [ ! -e test22.txt ] || [ $(grep -c "failed       : 0" test22.txt) -ne 1 ] || 
   [ ! -e test23.txt ] || [ $(grep -c "failed       : 0" test23.txt) -ne 1 ] || 
   [ ! -e test24.txt ] || [ $(grep -c "failed       : 0" test24.txt) -ne 1 ]; then
echo "nvme complience - failure"
else
echo "nvme complience - success"
fi

