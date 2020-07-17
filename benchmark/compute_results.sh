#!/bin/bash

names="fpc gorilla xor"

for c_name in $names
do 
    compressor='../test/memory/'$c_name'.o'

    dataset='AMPd'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done

    dataset='BAR-CRAWL'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done

    dataset='MAX-PLANCK'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done

    dataset='MSRC-12'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done

    dataset='OXFORD'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done

    dataset='PAMAP'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done
    
    dataset='UCI-GAS'
    echo "compr_speed,decompr_speed" >> results/$c_name-$dataset.csv
    for i in {1..10}
    do
        echo $c_name ' - ' $dataset ' - ' $i
        $compressor ../dataset/$dataset/$dataset.bin 1 >> results/$c_name-$dataset.csv
    done
done