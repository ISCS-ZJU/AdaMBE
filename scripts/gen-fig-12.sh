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


dataset_names_large=(StackOverflow IMDB BookCrossing Github)
dataset_abbs_large=(SO IM BX GH)
dataset_num_large=${#dataset_names_large[@]}

dataset_names_medium=(ActorMovies Wikipedia YouTube DBLP)
dataset_abbs_medium=(AM WC YG Pa)
dataset_num_medium=${#dataset_names_medium[@]}

result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-12. The expected time is 1000s." | tee -a $progress_file
data_file_a=./fig/fig-12/fig-12-a.data
data_file_b=./fig/fig-12/fig-12-b.data


rm $data_file_a
rm $data_file_b
echo "# Serie         AdaMBEFinder    AdaMBEFinderRand        AdaMBEFinderUC" >> $data_file_a
echo "# Serie         AdaMBEFinder    AdaMBEFinderRand        AdaMBEFinderUC" >> $data_file_b
for ((i=0;i<dataset_num_medium;i++)) 
do
  dataset_name=${dataset_names_medium[i]}
  dataset_abb=${dataset_abbs_medium[i]}
  printf "%s " "$dataset_abb" >> $data_file_a
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-RAND on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 -o 1 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-UC on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 -o 3 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  
  echo "" >> $data_file_a
done


for ((i=0;i<dataset_num_large;i++)) 
do
  dataset_name=${dataset_names_large[i]}
  dataset_abb=${dataset_abbs_large[i]}
  printf "%s " "$dataset_abb" >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-RAND on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 -o 1 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-UC on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7 -o 3 )
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_b
  
  echo "" >> $data_file_b
done
