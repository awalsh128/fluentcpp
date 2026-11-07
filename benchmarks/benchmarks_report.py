#!/usr/bin/env python3

import json
import sys


def delta_pct(x, y):
    return '%.2f' % ((x - y) / y)


def timings_from_json(filepath):
    results = json.load(open(filepath))
    timings = dict()

    for benchmark in results['benchmarks']:
        fqname = benchmark['name']
        name_path, size = fqname.split('/')
        _, fname, type = name_path.split('_')
        timings[(fname, type, size)] = int(benchmark['cpu_time'])

    return timings


def deltas(timings_fcpp, timings_baseline):
    lines = ["function,type,size,delta"]

    for key, cpu_time_fcpp in timings_fcpp.items():
        (fname, type, size) = key
        cpu_time_baseline = timings_baseline[key]
        delta = delta_pct(cpu_time_baseline, cpu_time_fcpp)
        lines.append(f'{fname},{type},{size},{delta}')

    return lines


def main(argv):
    timings_fcpp = timings_from_json(argv[0])
    timings_baseline = timings_from_json(argv[1])
    lines = deltas(timings_fcpp, timings_baseline)
    print('\n'.join(lines))


if __name__ == '__main__':
    main(sys.argv[1:])
