#!/bin/bash

for ns in 1 2 4 8 12 16 20 24 28 32 50 64 80 100 128 256; do
    echo "Testing with ns=$ns..."
    # Run raytracer and capture timing output
    output=$(./raytracer_cuda $ns 2>&1 | grep "took")
    # Extract just the number from "took X.XXX seconds."
    time=$(echo "$output" | grep -oP '\d+\.\d+')
    echo "$ns, $time" >> benchmark_results.csv
    echo "  Time: $time seconds"
done
