#!/bin/bash

set -e

./benchmarks_baseline --benchmark_out=benchmarks_baseline.json --benchmark_out_format=json
sleep 5
printf "\n"
./benchmarks --benchmark_out=benchmarks.json --benchmark_out_format=json
printf "\n"

mkdir -p ../../benchmarks/reports
LOG=../../benchmarks/reports/$(date +"%Y-%m-%d-benchmark_deltas").log
./benchmarks_report.py benchmarks.json benchmarks_baseline.json > $LOG

printf "\n"
cat $LOG
