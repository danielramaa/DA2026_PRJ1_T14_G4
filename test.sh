#!/bin/bash

INPUT_DIR="inputs"
OUTPUT_DIR="outputs"
BINARY="./cmake-build-debug/DA2026_PRJ1_T14_G3"

mkdir -p $OUTPUT_DIR

for f in $INPUT_DIR/*.csv; do
    filename=$(basename "$f" .csv)
    echo "Testing $filename..."
    $BINARY -b "$f" "$OUTPUT_DIR/out_$filename.csv"
done

echo "Done! Check outputs/ folder."