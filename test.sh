#!/bin/bash

./trust-ssh.sh

# Define an array of values for N
N_values=(1000 10000 100000 500000 1000000 5000000 10000000 50000000)

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

