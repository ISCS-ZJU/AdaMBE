#! /bin/bash

if [ ! -d "./bin/" ]
then
  mkdir ./bin/
fi

if [ ! -f "./bin/MBE_ALL" ]
then
  cd ./src || exit
  mkdir build
  cd build || exit
  cmake ..
  make
  mv MBE_* ../../bin/
  cd ../../
fi

