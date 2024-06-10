#! /bin/bash
if [ ! -d "./bin" ]
then
  mkdir ./bin
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


result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-11. The expected time is 20000s." | tee -a $progress_file
data_file=./fig/fig-11/fig-11.data


rm $data_file
echo "# tau_num   Github  BookCrossing" >> $data_file
for i in 0 4 8 16 24 32 40 48 56 64 80 96 112 128 144 160 176 192 208 224 240 256 
do
  printf "%s " "$i" >> $data_file
  
  dataset_name="Github"
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running MBE_$i on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_$i -i "./datasets/${dataset_name}.adj" -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  dataset_name="BookCrossing"
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running MBE_$i on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_$i -i "./datasets/${dataset_name}.adj" -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file

  echo "" >> $data_file
done
