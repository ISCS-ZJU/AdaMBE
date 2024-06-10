#! /bin/bash

if [ ! -f "./bin/MBE_ALL" ]
then
  cd ./src || exit
  mkdir build
  cd build || exit
  cmake ..
  make
  mv MBE_ALL ../../bin/
  cd ../../
fi

