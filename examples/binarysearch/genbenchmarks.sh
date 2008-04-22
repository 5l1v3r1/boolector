#!/bin/bash
limit=256
for ((size=4;size<=$limit;size*=2))
do
  header=1
  if [[ $size -lt 10 ]]; then
    sizestring="00"$size
  elif [[ $size -lt 100 ]]; then
    sizestring="0"$size
  else
    sizestring=$size
  fi
  filename=binarysearch32s$sizestring".smt"
  ./binarysearch 32 $size | boolector -rwl0 -ds | while read line
  do
    if [[ $header -eq 1 ]]; then
      echo "(benchmark $filename" > $filename
      echo ":source {" >> $filename
      echo "We write an arbitrary value into an array, assume that the array is sorted," >> $filename
      echo "and finally verify that the binary search algorithm always finds this value." >> $filename
      echo "Bit-width of elements: 32" >> $filename
      echo "Size of array: $size elements" >> $filename
      echo "" >> $filename
      echo -n "Contributed by Robert Brummayer " >> $filename
      echo "(robert.brummayer@gmail.com)." >> $filename
      echo "}" >> $filename
      if [[ $overlap -eq 1 ]]; then
	echo ":status sat" >> $filename
      else
	echo ":status unsat" >> $filename
      fi
      echo ":category { crafted }" >> $filename
      header=0
    else
      echo $line >> $filename
    fi
  done
done
