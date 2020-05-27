filename='dataset/AMPd/AMPd.bin'
echo $filename
for i in {1..10}; do zstd -b $filename; done > $filename.txt