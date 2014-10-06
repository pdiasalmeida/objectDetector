#!/bin/bash
./evaluator/evaluate -r traffic/proibPes_out/proibPes.dat -gt traffic/proibPes/gt.txt >> res_proibPes.txt
./evaluator/evaluate -r traffic/proibTran_out/proibTran.dat -gt traffic/proibTran/gt.txt >> res_proibTran.txt
./evaluator/evaluate -r traffic/proibUlt_out/proibUlt.dat -gt traffic/proibUlt/gt.txt >> res_proibUlt.txt
./evaluator/evaluate -r traffic/proibUltP_out/proibUltP.dat -gt traffic/proibUltP/gt.txt >> res_proibUltP.txt
./evaluator/evaluate -r traffic/slimit20_out/slimit20.dat -gt traffic/slimit20/gt.txt >> res_slimit20.txt
./evaluator/evaluate -r traffic/slimit30_out/slimit30.dat -gt traffic/slimit30/gt.txt >> res_slimit30.txt
./evaluator/evaluate -r traffic/slimit50_out/slimit50.dat -gt traffic/slimit50/gt.txt >> res_slimit50.txt
./evaluator/evaluate -r traffic/slimit60_out/slimit60.dat -gt traffic/slimit60/gt.txt >> res_slimit60.txt
./evaluator/evaluate -r traffic/slimit70_out/slimit70.dat -gt traffic/slimit70/gt.txt >> res_slimit70.txt
./evaluator/evaluate -r traffic/slimit80_out/slimit80.dat -gt traffic/slimit80/gt.txt >> res_slimit80.txt
./evaluator/evaluate -r traffic/slimit100_out/slimit100.dat -gt traffic/slimit100/gt.txt >> res_slimit100.txt
./evaluator/evaluate -r traffic/slimit120_out/slimit120.dat -gt traffic/slimit120/gt.txt >> res_slimit120.txt
