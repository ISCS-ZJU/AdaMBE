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

dataset_names=(Unicode UCforum MovieLens Teams ActorMovies Wikipedia YouTube StackOverflow DBLP IMDB BookCrossing Github)
dataset_abbs=(UL UF Mti TM AM WC YG SO Pa IM  BX GH)
dataset_num=${#dataset_names[@]}


# figure 6: running on a machine with 96-core CPUs and a GPU
result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-8. The expected time is 350000s." | tee -a $progress_file
data_file_a=./fig/fig-8/fig-8-a.data
data_file_b=./fig/fig-8/fig-8-b.data


rm $data_file_a
rm $data_file_b
echo "# Serie AdaMBEFinder FmbeFinder PmbeFinder ooMBEAFinder ParAdaMBEFinder ParMBE GMBE" >> $data_file_a
echo "# Serie AdaMBEFinder FmbeFinder PmbeFinder ooMBEAFinder ParAdaMBEFinder ParMBE GMBE" >> $data_file_b
for ((i=0;i<dataset_num;i++)) 
do
  dataset_name=${dataset_names[i]}
  dataset_abb=${dataset_abbs[i]}
  printf "%s " "$dataset_abb" >> $data_file_a
  printf "%s " "$dataset_abb" >> $data_file_b
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running FMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 3)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running PMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i ./datasets/${dataset_name}.adj -s 4)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ooMBEA on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbbp "./datasets/${dataset_name}.adj")
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParAdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 8 -t 96)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running ParMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/mbe_test "./datasets/${dataset_name}.adj" 96)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running GMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_GPU -i "./datasets/${dataset_name}.adj" -s 2 -t 1 -o 1 -f)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $8 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $9 }' >> $data_file_b
  echo >> $data_file_a
  echo >> $data_file_b
done
