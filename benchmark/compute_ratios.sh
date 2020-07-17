#!/bin/bash

echo "compressor,file,RATIO" >> results/RATIO.csv

names="fpc gorilla xor"

for c_name in $names
do 
    compressor='../test/memory/'$c_name'.o'
    dataset='AMPd'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='BAR-CRAWL'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='MAX-PLANCK'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='MSRC-12'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='OXFORD'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='PAMAP'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv

    dataset='UCI-GAS'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> results/RATIO.csv 
done