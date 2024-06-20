# Abstract
Maximal biclique enumeration (MBE) is a crucial problem in bipartite graph analysis, with broad applications across various domains.
Recent studies leverage multiple adjacency lists to represent bipartite graphs and rely on extensive set intersections to solve the MBE problem. 
However, these studies prioritize algorithmic optimization and ignore the inherent inefficiencies caused by data structure representation. 
These inefficiencies include the costly set intersections on adjacency lists and the redundancy caused by repeatedly accessing the complete neighbors of vertices. 
As a result, existing MBE approaches are limited to handling small bipartite graphs containing less than 56 million maximal bicliques.
To address this limitation, we propose two key optimizations for MBE. Firstly, we introduce the bitmap-based dynamic subgraph approach to expedite set intersections in MBE. 
Secondly, we propose the local neighbor caching approach to minimize redundancy.
Finally, we integrate these optimizations to present an Adaptive Maximal Biclique Enumeration (AdaMBE) algorithm. 
Experimental results illustrate that AdaMBE consistently achieves 1.6X-49.7X faster performance than its closest competitor. 
AdaMBE successfully enumerates all 19 billion maximal bicliques on the TVTropes dataset, demonstrating its scalability and efficiency.

# Try out AdaMBE
## Hardware requirements
A machine with GPUs and multi-cores CPU.
## Software Dependencies
- GNU Make 4.2.1
- CMake 3.22.0
- CUDA toolkit 11.7
- GCC/G++ 10.3.0
- Python 2.7.18
- Python packages: zplot 1.41, pathlib 1.0.1
- C++ library: libtbb-dev 2020.1-2, oneTBB 1.1
- Ubuntu apt package: Ghostscript, Texlive-extra-utils
- Nvidia driver 510.85.02
- Docker 20.10.10
## Deploy with docker image
We provide a docker image for the convenience to deploy this project. You should install some packages and download the docker image with the following commands 
before you run the docker image.
```
sudo apt install nvidia-docker2
docker pull fhxu00/adambe
```
To run the docker image, you should execute the following command. To measure the performance of GMBE, assure that the host machine has installed the nvidia driver with the version mentioned above.
```
docker run -it --gpus all --name adambe-test fhxu00/adambe bash
```
We have prepared the source code in the directory `~/AdaMBE-public` and downloaded all testing datasets in the docker image. Now you just need to compile the source code and run the testing scripts in the docker image as follows.

## Compiling
Using the following commands, one can easily compile the AdaMBE and baselines. The generated executable file is located at `bin/`.
```
# Get source code
git clone --recursive [repository_url]
cd AdaMBE-public 

# Compiling AdaMBE
bash ./scripts/compile-adambe.sh 

# Compiling baselines. [GPU_TYPE] denote the GPU used by GMBE (refer to https://github.com/fhxu00/MBE-GPU.git). 
bash ./scripts/compile-baselines.sh [GPU_TYPE] 
```

## Dataset preparing
For convenience, we provide a script to download and preprocess datasets. You can run the following command and you will find 
the preprocessed datasets under the new directory `datasets/`. 
```
bash ./preprocess/prepare_dataset.sh
```

## Running

You can run AdaMBE with the following command-line options.
```
./bin/MBE_ALL 
 -i: The path of input dataset file.
 -s: Select one AdaMBE version to run. 5: AdaMBE-BIT, 6: AdaMBE-LN, 7: AdaMBE, 8: ParAdaMBE.
 -t: Number of threads used to run AdaMBE, only useful for ParAdaMBE.
 -o: The ordering technique used. 1: random, 2: increasing, 3: unilateral order mentioned in ooMBEA.
```
## Experimental workflow
We provide the scripts to generate the experimental results of Figure 8-14 in the directory `scripts/`. You can execute the scripts as following.
```
# Running on a machine with a 96-core CPU and a GPU
bash ./scripts/gen-fig-8.sh 

# Running on any machine
bash ./scripts/gen-fig-9.sh

# Running on any machine
bash ./scripts/gen-fig-10.sh 

# Running on any machine
bash ./scripts/gen-fig-11.sh 

# Running on any machine
bash ./scripts/gen-fig-12.sh 

# Running on any machine
bash ./scripts/gen-fig-13.sh 

# Running on a machine with a 96-core CPU
bash ./scripts/gen-fig-14.sh 

```
We provide the script to generate figures in the directory `fig/` with the results generated in above. You can execute the script as following.
```
cd fig/
bash genfig.sh
```
Then you will find the figures under the directory `fig/`.
