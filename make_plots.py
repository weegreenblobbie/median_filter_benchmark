"""

1D Moving Median Filter Benchmark

Copyright (c) 2016 to present, Nick Hilton

"""

import argparse
import datetime

from matplotlib import pyplot as plt


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        'benchmark_stdout',
        help = 'The text output from the benchmark'
    )

    args = parser.parse_args()

    #--------------------------------------------------------------------------
    # read in file

    with open(args.benchmark_stdout, 'r') as fd:
        lines = fd.readlines()

    # skip lines until after TABLE

    i = 0;

    for l in lines:

        i += 1

        if "TABLE" in l:
            break;

    x_axis = []

    stats = {}

    titles = lines[i].split()

    assert titles[0] == 'Window'

    keys = titles[1:]

    i += 1

    #--------------------------------------------------------------------------
    # parse in each table line

    for l in lines[i:]:

        tokens = l.split()

        x_axis.append(int(tokens[0]))

        for j in xrange(len(keys)):

            k = keys[j]

            v = int(tokens[j+1])

            try:
                stats[k].append(v)
            except:
                stats[k] = [v]

#~    for k,v in stats.iteritems():
#~        print "%s : %s" % (k, v)

    #--------------------------------------------------------------------------
    # plot everyting

    save_plot(x_axis, stats, 'stats_all.png')

    del stats['NthElement']

    save_plot(x_axis, stats, 'stats_zoom.png')

    plt.show()


def save_plot(x_axis, stats, fout):

    fig = plt.figure(figsize = [12,12])

    for k, v in stats.iteritems():

        plt.plot(x_axis, v, label = k, marker = 'o')

    plt.grid(True)

    plt.xlabel('Median Size')
    plt.ylabel('Execution Time (ms)')

    d = datetime.datetime.now()

    timestamp = '%d-%02d-%02d' % (d.year, d.month, d.day)

    plt.title('1D Moving Median Filter Benchmark (%s)' % timestamp)

    ax = plt.axes()

    ax.margins(0.05, tight=True)

    plt.legend(loc = 'upper left')

    plt.savefig(fout)




if __name__ == "__main__": main()
