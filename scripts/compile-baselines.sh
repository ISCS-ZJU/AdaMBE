#! /bin/bash
if [ ! -d "./bin" ]
then
  mkdir ./bin
fi

if [ ! -f "./bin/mbbp" ]
then
  cd ./baselines || exit
  cd oombea || exit
  mkdir build
  cd build || exit
  cmake ..
  make 
  mv mbbp ../../../bin/
  cd ../../../
fi

if [ ! -f "./bin/mbe_test" ]
then
  cd ./baselines || exit
  cd parallel-mbe || exit
  mkdir build
  cd build || exit
  cmake ..
  make
  mv mbe_test ../../../bin/
  cd ../../../
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

