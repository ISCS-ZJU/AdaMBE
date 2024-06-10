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


result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-14. The expected time is 50000s." | tee -a $progress_file
data_file_a=./fig/fig-14/fig-14-a.data
data_file_b=./fig/fig-14/fig-14-b.data


rm $data_file_a
rm $data_file_b
echo "#   thread_num  ParMBE  ParAdaMBEFinder" >> $data_file_a
echo "#   thread_num  ParMBE  ParAdaMBEFinder" >> $data_file_b
for i in 4 8 16 32 64 96 
do
  printf "%s " "$i" >> $data_file_a
  printf "%s " "$i" >> $data_file_b
  
  dataset_name="Github"
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParAdaMBE with $i threads on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_$i -i "./datasets/${dataset_name}.adj" -s 8 -t $i)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParMBE with $i threads on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbe_test "./datasets/${dataset_name}.adj" $i)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  
  dataset_name="CebWiki"
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParAdaMBE with $i threads on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_$i -i "./datasets/${dataset_name}.adj" -s 8 -t $i)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_b

  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParMBE with $i threads on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbe_test "./datasets/${dataset_name}.adj" $i)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_b

  echo "" >> $data_file_a
  echo "" >> $data_file_b
done
