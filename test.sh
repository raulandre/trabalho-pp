#!/bin/bash

./trust-ssh.sh

# Define an array of values for N
N_values=(
    2000
    4000
    8000
    16000
    32000
    64000
    128000
    256000
    512000
    1024000
    2048000
    4096000
    8192000
    16384000
    32768000
    65536000
    131072000
    262144000
    524288000
    1048576000
    2097152000
    4194304000
    8388608000
    16777216000
    33554432000
    67108864000
    134217728000
    268435456000
    536870912000
    1073741824000
    2147483648000
)

# Clean and set up results directory
rm -rf results/
mkdir results

# Loop through each value in the N array
for N in "${N_values[@]}"; do
	make clean
	make_out=$(make N=$N 2>&1)

	if [ $? -eq 0 ]; then
		printf "=============================\n"
		printf "Running sequential version for N=$N...\n"
		(time ./seq) 2> results/${N}_seq.txt
		printf "=============================\n"
		printf "Running distributed version for N=$N...\n"
		(time mpirun --mca btl tcp,self --host mpi-node-1,mpi-node-2,mpi-node-3,mpi-node-4 -np 4 ./dist) 1> results/${N}_dist.txt
		printf "=============================\n"
	else
		echo "Build failed for N=$N: $make_out"
	fi
done

