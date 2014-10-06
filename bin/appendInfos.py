#!/usr/bin/python

from sys import argv, exit

class InfoAppend:
    def append(self, outinfo, files):
        fw = open(outinfo, 'w')
        for path in files:
            fr = open(path, 'r')
            for line in fr:
                fw.write(line)

if __name__ == "__main__":
    if len(argv) < 3:
        print "usage: %s <path to '.info' output file> <path to info files to append> ..." % argv[0]
        exit(1)
        
    ap = InfoAppend()
    ap.append(argv[1], argv[2:])
