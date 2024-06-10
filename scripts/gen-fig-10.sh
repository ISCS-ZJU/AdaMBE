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


dataset_names=(Unicode UCforum MovieLens Teams ActorMovies Wikipedia YouTube StackOverflow DBLP IMDB BookCrossing Github)
dataset_abbs=(UL UF Mti TM AM WC YG SO Pa IM BX GH)
dataset_num=${#dataset_names[@]}

result_file="./scripts/results.txt"
progress_file="./scripts/progress.txt"
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo $cur_time "Generating fig-10. The expected time is 100000s." | tee -a $progress_file
data_file_a=./fig/fig-10/fig-10-a/fig-10-a.data
data_file_b=./fig/fig-10/fig-10-b/fig-10-b.data
data_file_c=./fig/fig-10/fig-10-c/fig-10-c.data
data_file_d=./fig/fig-10/fig-10-a/fig-10-d.data


rm $data_file_a
rm $data_file_b
rm $data_file_c
rm $data_file_d
echo "# Serie Baseline        BDS     LNC     AdaMBE" >> $data_file_a
echo "# Serie Baseline        BDS     LNC     AdaMBE" >> $data_file_b
echo "# Serie         Baseline        AdaMBE_NC       Overlap" >> $data_file_c
echo "# Serie Baseline        AdaMBE_BDS      common" >> $data_file_d
for ((i=0;i<dataset_num;i++)) 
do
  dataset_name=${dataset_names[i]}
  dataset_abb=${dataset_abbs[i]}
  printf "%s " "$dataset_abb" >> $data_file_a
  printf "%s " "$dataset_abb" >> $data_file_b
  printf "%s " "$dataset_abb" >> $data_file_c
  printf "%s " "$dataset_abb" >> $data_file_d

  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running Baseline on dataset" ${dataset_name} | tee -a $progress_file
  output_baseline=$(./bin/MBE_0 -i "./datasets/${dataset_name}.adj" -s 5)
  echo $output_baseline >> ${result_file} 
  echo $output_baseline | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output_baseline | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  maximal_nodes=$(echo $output_baseline | awk -F "," 'NR<=1 {printf "%s ", $5 }')
  all_nodes_baseline=$(echo $output_baseline | awk -F "," 'NR<=1 {printf "%s ", $6 }')
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-BIT on dataset" ${dataset_name} | tee -a $progress_file
  output_bit=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 5)
  echo $output_bit >> ${result_file} 
  echo $output_bit | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output_bit | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE-LN on dataset" ${dataset_name} | tee -a $progress_file
  output_ln=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 6)
  echo $output_ln >> ${result_file} 
  echo $output_ln | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output_ln | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b
  all_nodes_ln=$(echo $output_ln | awk -F "," 'NR<=1 {printf "%s ", $6 }')
  overlap=$(echo "scale=2;$maximal_nodes / $all_nodes_baseline" | bc)
  ln=$(echo "scale=2;$all_nodes_ln / $all_nodes_baseline" | bc)
  printf "1 %s %s" "$ln" "$overlap" >> $data_file_c
  
  cur_time=$(date "+%Y-%m-%d %H:%M:%S")
  echo $cur_time "Running AdaMBE on dataset" ${dataset_name} | tee -a $progress_file
  output=$(./bin/MBE_ALL -i "./datasets/${dataset_name}.adj" -s 7)
  echo $output >> ${result_file} 
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $3 }' >> $data_file_a
  echo $output | awk -F "," 'NR<=1 {printf "%s ", $4 }' >> $data_file_b

  echo "" >> $data_file_a
  echo "" >> $data_file_b
  echo "" >> $data_file_c
  echo " " >> $data_file_d
done
