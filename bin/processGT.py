#!/usr/bin/python

from sys import argv, exit
import os
import shutil

class Processor:
    def process(self, gtFile, outDir, classe):
        fr = open(gtFile, 'r')
        fw = open(outDir+'/gt.txt', 'w')
        for line in fr:
            args = line.split(';')
            if( (len(args) == 6) ):
                try:
                    if( int(args[5]) == int(classe) ):
                        s = []
                        s.append('./')
                        s.append(args[0])
                        s.append(' ')
                        s.append(args[1])
                        s.append(' ')
                        s.append(args[2])
                        s.append(' ')
                        s.append(str(int(args[3])-int(args[1])))
                        s.append(' ')
                        s.append(str(int(args[4])-int(args[2])))
                        s.append(' ')
                        s.append(args[5])
                        fw.write(''.join(s))
                        shutil.copy(self.getFileDir(gtFile)+'/'+args[0], outDir+'/'+args[0])
                except ValueError:
                    continue

    def getFileName(self, filePath):
        return filePath.split('/')[-1]

    def getFileDir(self, filePath):
        return filePath[:filePath.rfind('/')]

if __name__ == "__main__":
    if len(argv) != 4:
        print "usage: %s <path to groundtruth file> <path to out directory> <class of sign to process>" % argv[0]
        exit(1)
        
    p = Processor()
    if not os.path.exists(argv[2]):
        os.makedirs(argv[2])
    p.process(argv[1], argv[2], argv[3])
