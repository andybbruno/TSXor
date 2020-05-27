compressor='./test_FPC.o'
cname='FPC'

echo 'AMPD'
for i in {1..10}; do $compressor dataset/AMPd/AMPd.bin 1; done > res/AMPd_$cname.txt

echo 'MSRC-12'
for i in {1..10}; do $compressor dataset/MSRC-12/MSRC-12.bin 1; done > res/MSRC_$cname.txt

echo 'PAMAP'
for i in {1..10}; do $compressor dataset/PAMAP/PAMAP.bin 1; done > res/PAMAP_$cname.txt

echo 'UCI-GAS'
for i in {1..10}; do $compressor dataset/UCI-GAS/UCI-GAS.bin 1; done > res/UCI-GAS_$cname.txt