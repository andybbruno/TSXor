#!/bin/bash

# echo "compressor,file,RATIO" >> XOR_RATIO.csv

names="xor10 xor25 xor50 xor75 xor100"

for c_name in $names
do 
    compressor='../test/memory/'$c_name'.o'
    dataset='AMPd'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='BAR-CRAWL'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='MAX-PLANCK'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='MSRC-12'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='OXFORD'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='PAMAP'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
    dataset='UCI-GAS'
    echo $c_name ' - ' $dataset
    $compressor ../dataset/$dataset/$dataset.bin 1 >> XOR_RATIO.csv
    echo >> XOR_RATIO.csv
done