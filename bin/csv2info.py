#!/usr/bin/python

from sys import argv, exit

class CsvConverter:
    def convCsv(self, pathCsv, pathInfo, imgDir):
        fr = open(pathCsv, 'r')
        fw = open(pathInfo, 'w')

        for line in fr:
            args = line.split(';')
            if( (len(args) == 8) ):
                try:
                    s = []
                    s.append('./')
                    s.append(imgDir)
                    s.append('/')
                    s.append(args[0])
                    s.append(' 1 ')
                    s.append(args[3])
                    s.append(' ')
                    s.append(args[4])
                    s.append(' ')
                    s.append(str(int(args[5])-int(args[3])))
                    s.append(' ')
                    s.append(str(int(args[6])-int(args[4])))
                    s.append('\n')
                    fw.write(''.join(s))
                except ValueError:
                    continue

if __name__ == "__main__":
    if len(argv) != 4:
        print "usage: %s <path to '.csv' file> <path to save '.info' file> <image directory>" % argv[0]
        exit(1)
        
    conv = CsvConverter()
    conv.convCsv(argv[1], argv[2], argv[3])
