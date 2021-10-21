#!/usr/bin/env python3

import json
import sys


def delta_pct(x, y):
    return '%.2f' % ((x - y) / y)


def from_json(filepath):
    results = json.load(open(filepath))
    timings = dict()
    lines = list()

    for benchmark in results['benchmarks']:
        fqname = benchmark['name']
        name_path, size = fqname.split('/')
        _, fname, type, competitor = name_path.split('_')
        key = (fname, type, size)
        timing = timings.get(key, dict())
        timing[competitor.lower()] = int(benchmark['cpu_time'])
        timings[key] = timing

    for key, values in timings.items():
        print(values)
        delta = delta_pct(values['std'], values['fcpp'])
        fname, type, size = key
        lines.append(f'{fname} {type} {size} {delta}')

    return lines


def main(argv):
    lines = from_json(argv[0])
    print('\n'.join(lines))


if __name__ == '__main__':
    main(sys.argv[1:])
