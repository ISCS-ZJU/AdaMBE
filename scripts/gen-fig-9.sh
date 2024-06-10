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

if [ ! -f "./bin/MBE_GPU" ]
then
  cd ./baselines || exit
  cd MBE-GPU || exit
  mkdir build
  cd build || exit
  cmake .. -DGPU_TYPE=$GPU_TYPE
  make
  mv MBE_GPU ../../../bin/
  cd ../../../
fi

# figure 6: running on a machine with 96-core CPUs and a GPU
result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-9. The expected time is 70 hours." | tee -a $progress_file


  data_file=./fig/fig-9/fig-9-a.data
  echo "# Serie time" >> $data_file
  rm $data_file
  dataset_name="CebWiki"
  printf "%s " "AdaMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "FMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running FMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 3)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "PMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running PMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 4)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ooMBEA" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ooMBEA on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbbp "./datasets/${dataset_name}.graph")
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ParAdaMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParAdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 8 -t 96)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ParMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbe_test "./datasets/${dataset_name}.graph" 96)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s 0" "GMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running GMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_GPU -i "./datasets/${dataset_name}.adj" -s 2 -t 1 -o 1 -f)
  echo $output >> ${result_file} 
  echo >> $data_file

  data_file=./fig/fig-9/fig-9-b.data
  echo "# Serie time" >> $data_file
  rm $data_file
  dataset_name="TVTropes"
  printf "%s " "AdaMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "FMBE  364619896 172800 " >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running FMBE on dataset" ${dataset_name} | tee -a $progress_file
  #output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 3)
  #echo $output >> ${result_file} 
  #echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "PMBE 372095010 172800" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running PMBE on dataset" ${dataset_name} | tee -a $progress_file
  #output=$(./bin/MBE -i ./datasets/${dataset_name}.adj -s 4)
  #echo $output >> ${result_file} 
  #echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ooMBEA 477400425 172800" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ooMBEA on dataset" ${dataset_name} | tee -a $progress_file
  #output=$(./bin/mbbp "./datasets/${dataset_name}.graph")
  #echo $output >> ${result_file} 
  #echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ParAdaMBE" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParAdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 8 -t 96)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "ParMBE 1672629114  172800" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParMBE on dataset" ${dataset_name} | tee -a $progress_file
  #output=$(./bin/mbe_test "./datasets/${dataset_name}.graph" 96)
  #echo $output >> ${result_file} 
  #echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file
  
  printf "%s " "GMBE 13063719355 172800" >> $data_file
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running GMBE on dataset" ${dataset_name} | tee -a $progress_file
  #output=$(./bin/MBE_GPU -i "./datasets/${dataset_name}.adj" -s 2 -t 1 -o 1 -f)
  #echo $output >> ${result_file} 
  #echo $output | awk -F "," 'NR<=1 {printf "%s ", $8 }' >> $data_file
  echo >> $data_file

