#!/usr/bin/python

import os
import errno
from sys import argv, exit

import cv2

class BlurImages:
    pathFolder = str()
    detObjs = dict()

    def readFile(self, dirFolder):
        self.pathFolder = dirFolder.rstrip('/')
        pathFile = self.pathFolder+'/'+self.pathFolder.split('/')[-1][:-4]+".dat"

        fr = open(pathFile, 'r')
        for line in fr:
            args = line.split(',')
            key = args[0].rstrip()
            if key in self.detObjs:
                self.detObjs[key].append((int(args[2]),int(args[3]),int(args[4]),int(args[5])))
            else:
                if len(args) > 1:
                    self.detObjs[key] = [(int(args[2]),int(args[3]),int(args[4]),int(args[5]))]
                else:
                    self.detObjs[key] = []

        fr.close()

    def doBlur(self):
        imgFolder = self.pathFolder[:-4]+'/'
        dstFolder = self.pathFolder+"/temp/"

        try:
            os.makedirs(dstFolder)
        except OSError as exception:
            if exception.errno != errno.EEXIST:
                raise

        for key in self.detObjs:
            img = cv2.imread(imgFolder+key)

            for tupl in self.detObjs[key]:
                roi = img[tupl[1]:tupl[1]+tupl[3],tupl[0]:tupl[0]+tupl[2]]
                blur = cv2.GaussianBlur(roi,(15,15),100)
                img[tupl[1]:tupl[1]+tupl[3],tupl[0]:tupl[0]+tupl[2]] = blur

            cv2.imwrite(dstFolder+key,img)

    def printStruct(self):
        print self.detObjs

if __name__ == "__main__":
    if len(argv) != 2:
        print "usage: %s <path to process output folder>" % argv[0]
        exit(1)

    blr = BlurImages()
    blr.readFile(argv[1])
    blr.doBlur()
