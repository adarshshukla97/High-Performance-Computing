#!/bin/sh
echo "Running the script!!"
n=1 #!input file number
x=16 #! number of core
m=1

while [ $n -lt 2 ]
do
	echo "for serial " $n
	gcc -fopenmp MST_serial.c -w
	./a.out < in$n.txt
	
	echo
	echo "for parrallel " $n
	gcc -fopenmp MST_parallel.c -w
	./a.out $x < in$n.txt

	echo "------------------------------------------------"
	n=`expr $n \+ $m`

done	
