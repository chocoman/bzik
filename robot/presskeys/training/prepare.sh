#!/bin/bash
for symbol in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9; do
  i=0;
  cd ~/Documents/programovani/bzik/training/testset/$symbol
  pwd
  mkdir tmp
  for file in `ls *.png`; do
    mv $file tmp/img$i.png;
    i=$(($i+1));
  done;
  mv tmp/* .
  rmdir tmp
  echo $i > nSamples;
done;
