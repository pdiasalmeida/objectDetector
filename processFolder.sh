#!/bin/bash
detectObjects -d $1
avconv -framerate 12.87 -i $1_out/%05d.jpg -threads 8 -c:v libvpx -r 12.87 -b 614400 -s 480x270 $2
