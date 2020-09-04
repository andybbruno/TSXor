#!/bin/bash

names="xor10 xor25 xor50 xor75 xor100"

for c_name in $names
do 
    compressor='algo/'$c_name'.o'

    dataset='AMPd'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done

    dataset='BAR-CRAWL'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done

    dataset='MAX-PLANCK'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done

    dataset='MSRC-12'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done

    dataset='OXFORD'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done

    dataset='PAMAP'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done
    
    dataset='UCI-GAS'
    echo "compr_speed,decompr_speed" >> results/xor/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/xor/$c_name-$dataset.csv
    done
done