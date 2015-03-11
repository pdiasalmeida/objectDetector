#!/bin/bash
DIR=${1%/*}

./blurImages.py $DIR/BC_${DIR}_out
avconv -framerate 12.87 -i $DIR/BC_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/BC_${DIR}_out/BC_${DIR}.webm
rm -rf -- $DIR/BC_${DIR}_out/temp

./blurImages.py $DIR/BL_${DIR}_out
avconv -framerate 12.87 -i $DIR/BL_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/BL_${DIR}_out/BL_${DIR}.webm
rm -rf -- $DIR/BL_${DIR}_out/temp

./blurImages.py $DIR/BR_${DIR}_out
avconv -framerate 12.87 -i $DIR/BR_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/BR_${DIR}_out/BR_${DIR}.webm
rm -rf -- $DIR/BR_${DIR}_out/temp

./blurImages.py $DIR/FC_${DIR}_out
avconv -framerate 12.87 -i $DIR/FC_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/FC_${DIR}_out/FC_${DIR}.webm
rm -rf -- $DIR/FC_${DIR}_out/temp

./blurImages.py $DIR/FL_${DIR}_out
avconv -framerate 12.87 -i $DIR/FL_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/FL_${DIR}_out/FL_${DIR}.webm
rm -rf -- $DIR/FL_${DIR}_out/temp

./blurImages.py $DIR/FR_${DIR}_out
avconv -framerate 12.87 -i $DIR/FR_${DIR}_out/temp/%05d.jpg -threads auto -c:v libvpx -r 12.87 -b 256k -s 306x256 $DIR/FR_${DIR}_out/FR_${DIR}.webm
rm -rf -- $DIR/FR_${DIR}_out/temp
