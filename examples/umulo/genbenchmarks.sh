#!/bin/bash
inc=16
for ((bits=16; bits <= 512; bits+=inc))
do
  header=1
  if [[ $bits -lt 100 ]]; then
    bitsstring="0"$bits
  else 
    bitsstring=$bits
  fi
  filename=umulov1bw$bitsstring".smt"
  ./umulov1 $bits | boolector -rwl0 -ds | while read line
  do
    if [[ $header -eq 1 ]]; then
      echo "(benchmark $filename" > $filename
      echo ":source {" >> $filename
      echo "We verify the correctness of an unsigned multiplication" >> $filename
      echo "overflow detection unit as proposed in" >> $filename
      echo "\"Combined unsigned and two's complement saturating multipliers\"" >> $filename
      echo "by M. Schulte et al." >> $filename
      echo "" >> $filename
      echo "Bit-width: $bits" >> $filename
      echo "" >> $filename
      echo -n "Contributed by Robert Brummayer " >> $filename
      echo "(robert.brummayer@gmail.com)." >> $filename
      echo "}" >> $filename
      echo ":status unsat" >> $filename
      echo ":category { industrial }" >> $filename
      header=0
    else
      echo $line >> $filename
    fi
  done
  if [[ $bits -eq 128 ]]; then
    inc=32
  elif [[ $bits -eq 256 ]]; then
    inc=64
  fi
done

inc=16
for ((bits = 32; bits <= 1024; bits+=inc))
do
  header=1
  if [[ $bits -lt 100 ]]; then
    bitsstring="00"$bits
  elif [[ $bits -lt 1000 ]]; then 
    bitsstring="0"$bits
  else
    bitsstring=$bits
  fi
  filename=umulov2bw$bitsstring".smt"
  ./umulov2 $bits | boolector -rwl0 -ds | while read line
  do
    if [[ $header -eq 1 ]]; then
      echo "(benchmark $filename" > $filename
      echo ":source {" >> $filename
      echo "We verify a verification condition of an unsigned multiplication" >> $filename
      echo "overflow detection unit as proposed in" >> $filename
      echo "\"Combined unsigned and two's complement saturating multipliers\"" >> $filename
      echo "by M. Schulte et al." >> $filename
      echo "" >> $filename
      echo "Let n be the bit-width and r the result of the multiplication." >> $filename
      echo "If the n/2 most significant bits of the operands are zero, then." >> $filename
      echo "the overflow detection unit must not yield an overflow." >> $filename
      echo "" >> $filename
      echo "Bit-width: $bits" >> $filename
      echo "" >> $filename
      echo -n "Contributed by Robert Brummayer " >> $filename
      echo "(robert.brummayer@gmail.com)." >> $filename
      echo "}" >> $filename
      echo ":status unsat" >> $filename
      echo ":category { industrial }" >> $filename
      header=0
    else
      echo $line >> $filename
    fi
  done
  if [[ $bits -lt 256 ]]; then
    ((inc = 2 * inc))
  else
    inc=128
  fi
done
