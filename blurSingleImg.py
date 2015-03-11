#!/usr/bin/python

import os
import errno
from sys import argv, exit

import cv2

class BlurImages:
    def __init__(self, track, camera, frame, dst):
        track = track.rstrip('/')
        camera = camera.upper()
        frame = frame.zfill(5)
        day = track.split('(')[0]

        baseDir = "/home/paulo/projects/infoportugal/objectDetectorIP/"
        self.datFile = baseDir+day+"/"+track+"/"+camera+"_"+track+"_out/"+frame+".dat"
        self.imgFile = baseDir+day+"/"+track+"/"+camera+"_"+track+"/"+frame+".jpg"
        self.dstFolder = dst.rstrip('/')+"/"+day+"/"+track+"/"+camera+"_"+track
        self.dstFile = self.dstFolder+"/"+frame+".jpg"

        self.detObjs = []

    def readFile(self):
        fr = open(self.datFile, 'r')
        for line in fr:
            args = line.split(',')
            self.detObjs.append((int(args[2]),int(args[3]),int(args[4]),int(args[5])))

        fr.close()

    def doBlur(self):
        try:
            os.makedirs(self.dstFolder)
        except OSError as exception:
            if exception.errno != errno.EEXIST:
                raise

        img = cv2.imread(self.imgFile)

        for tupl in self.detObjs:
            roi = img[tupl[1]:tupl[1]+tupl[3],tupl[0]:tupl[0]+tupl[2]]
            blur = cv2.GaussianBlur(roi,(15,15),100)
            img[tupl[1]:tupl[1]+tupl[3],tupl[0]:tupl[0]+tupl[2]] = blur

        cv2.imwrite(self.dstFile,img)

    def printStruct(self):
        print self.detObjs

if __name__ == "__main__":
    if len(argv) != 5:
        print "usage: %s <track> <camera> <frame> <outDir>" % argv[0]
        exit(1)

    blr = BlurImages(argv[1], argv[2], argv[3], argv[4])
    blr.readFile()
    blr.doBlur()
