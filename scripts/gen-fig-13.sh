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

#dataset_names=(LiveJournal10 LiveJournal20 LiveJournal30 LiveJournal40 LiveJournal50)
dataset_names=(Unicode Unicode Unicode Unicode Unicode)
dataset_abbs=(LJ10 LJ20 LJ30 LJ40 LJ50)
dataset_num=${#dataset_names[@]}

# figure 6: running on a machine with 96-core CPUs and a GPU
result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-13. The expected time is 200000s." | tee -a $progress_file
data_file=./fig/fig-13/fig-13.data
data_file_0=./fig/fig-13/fig-13-0.data
data_file_1=./fig/fig-13/fig-13-1.data
data_file_2=./fig/fig-13/fig-13-2.data
data_file_3=./fig/fig-13/fig-13-3.data


rm $data_file
rm $data_file_0
rm $data_file_1
rm $data_file_2
rm $data_file_3
echo "# series AdaMBEFinder FmbeFinder  PmbeFinder  ooMBEAFinder" >> $data_file
echo "# AdaMBEFinder" >> $data_file_0
echo "# FmbeFinder" >> $data_file_1
echo "# PmbeFinder" >> $data_file_2
echo "# ooMBEAFinder" >> $data_file_3
for ((i=0;i<dataset_num;i++)) 
do
  dataset_name=${dataset_names[i]}
  dataset_abb=${dataset_abbs[i]}
  printf "%s " "$dataset_abb" >> $data_file

  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_0
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running FMBE on dataset" ${dataset_name} | tee -a $progress_file
  if [[ $i -lt 2 ]];
  then
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 3)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_1
  else
  printf "172800 " >> $data_file
  fi
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running PMBE on dataset" ${dataset_name} | tee -a $progress_file
  if [[ $i -lt 1 ]];
  then
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 4)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_2
  else
  printf "172800 " >> $data_file
  fi
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ooMBEA on dataset" ${dataset_name} | tee -a $progress_file
  if [[ $i -lt 4 ]];
  then
  output=$(./bin/mbbp "./datasets/${dataset_name}.graph")
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_3
  else
  printf "172800 " >> $data_file
  fi
  
  echo >> $data_file
  echo >> $data_file_0
  echo >> $data_file_1
  echo >> $data_file_2
  echo >> $data_file_3
done
