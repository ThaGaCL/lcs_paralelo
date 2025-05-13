#!/bin/bash

export LC_NUMERIC="en_US.UTF-8"

PAR_DIR="paralelo"
SEQ_DIR="sequencial"

options=("small" "medium" "big" "huge")
num_iterations=20
num_threads=(1 2 4 8)

# Initialization 
cd $PAR_DIR 
make clean > /dev/null && make > /dev/null
cd ..
cd $SEQ_DIR
make clean > /dev/null && make > /dev/null
cd ..

init_total() {
  echo "0.0"
}

for num in "${num_threads[@]}"; do 
  for option in "${options[@]}"; do
    # Generate files for parallel
    cd $PAR_DIR
    python3 generate_files.py "$option" > /dev/null
    cd ..

    echo "----------------------------------------"
    echo "Testing PARALLEL ($option) with $num threads"

    # Parallel timing with date
    cd $PAR_DIR
    par_total=$(init_total)
    for ((i=0; i<num_iterations; i++)); do
      start=$(date +%s.%N)
      ./lcs $num /dev/null 2> /dev/null
      end=$(date +%s.%N)
      runtime=$(echo "$end - $start" | bc -l)
      par_total=$(echo "$par_total + $runtime" | bc -l)
    done
    par_avg=$(echo "scale=3; $par_total / $num_iterations" | bc -l)
    printf "PARALLEL Average: %.3f s\n" $par_avg
    cd ..

    echo "----------------------------------------"
  done
done

# Sequential tests
for option in "${options[@]}"; do
    # Generate files for sequential
    cd $SEQ_DIR
    python3 generate_files.py "$option" > /dev/null
    cd ..

    echo "----------------------------------------"
    echo "Testing SEQUENTIAL ($option)"

    # Sequential timing with date
    cd $SEQ_DIR
    seq_total=$(init_total)
    for ((i=0; i<num_iterations; i++)); do
      start=$(date +%s.%N)
      ./lcs > /dev/null 2> /dev/null
      end=$(date +%s.%N)
      runtime=$(echo "$end - $start" | bc -l)
      seq_total=$(echo "$seq_total + $runtime" | bc -l)
    done
    seq_avg=$(echo "scale=3; $seq_total / $num_iterations" | bc -l)
    printf "SEQUENTIAL Average: %.3f s\n" $seq_avg
    cd ..

    echo "----------------------------------------"
done