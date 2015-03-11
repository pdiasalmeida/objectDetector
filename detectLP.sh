#!/bin/bash
DIR=${1%/*}

./detectObjects -d $DIR/BC_$DIR
./detectObjects -d $DIR/BL_$DIR
./detectObjects -d $DIR/BR_$DIR
./detectObjects -d $DIR/FC_$DIR
./detectObjects -d $DIR/FL_$DIR
./detectObjects -d $DIR/FR_$DIR
