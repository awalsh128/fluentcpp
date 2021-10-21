#!/bin/bash

set -e

./benchmarks --benchmark_out=benchmarks_results.json --benchmark_out_format=json
./benchmarks_delta.py benchmarks_results.json
