#!/bin/bash

echo ""
echo "=== Running My Engine Benchmark ==="
cd src
./raytracer
cd ..

echo ""
echo "=== Running RTIOW Benchmark ==="
cd benchmark
./benchmark
cd ..

echo ""
echo "=== Done ==="
