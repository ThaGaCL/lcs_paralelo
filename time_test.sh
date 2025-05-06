#!/bin/bash

export TIMEFORMAT='%5R'
export LC_NUMERIC="en_US.UTF-8"

PAR_DIR="paralelo"
SEQ_DIR="sequencial"

options=("original" "noRepeat" "noRepeatBig" "big" "huge")
num_iterations=100

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

for option in "${options[@]}"; do
  # Generate the files based on the option
  cd $PAR_DIR
  python3 generate_files.py "$option" > /dev/null
  cd ..
  cd $SEQ_DIR
  python3 generate_files.py "$option" > /dev/null
  cd ..

  echo "----------------------------------------"
  echo "Starting tests for $option"

  # Parallel version timing
  cd $PAR_DIR
  echo "Testing PARALLEL version for $option"
  par_total=$(init_total)
  for ((i=0; i<num_iterations; i++)); do  # Corrected loop
    runtime=$( { time ./lcs > /dev/null; } 2>&1 )
    par_total=$(printf "%s + %s\n" "$par_total" "$runtime" | bc -l)
  done
  par_avg=$(printf "scale=3; %s / $num_iterations\n" "$par_total" | bc -l)
  echo "PARALLEL Average: ${par_avg}s"
  cd ..

  # Sequential version timing
  cd $SEQ_DIR
  echo "Testing SEQUENTIAL version for $option"
  seq_total=$(init_total)
  for i in {1..$num_iterations}; do
    runtime=$( { time ./lcs > /dev/null; } 2>&1 )
    seq_total=$(printf "%s + %s\n" "$seq_total" "$runtime" | bc -l)
  done
  seq_avg=$(printf "scale=3; %s / $num_iterations\n" "$seq_total" | bc -l)
  echo "SEQUENTIAL Average: ${seq_avg}s"
  cd ..

  echo "Finished tests for $option"
  echo "----------------------------------------"
done