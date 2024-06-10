#! /bin/bash
if [ ! -d "./bin" ]
then
  mkdir ./bin
fi

if [ $# -eq 0 ]
then
  GPU_TYPE=A100
else
  GPU_TYPE=$1
fi

if [ ! -f "./bin/MBE_GPU" ]  
then
  cd ./baselines/MBE-GPU || exit
  mkdir build
  cd build || exit
  cmake .. -DGPU_TYPE=$GPU_TYPE
  make
  mv MBE_GPU* ../../../bin/
  cd ../../../
fi

