compressor='./test_xor_cache.o'

dataset='AMPd'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='BAR_CRAWL'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='MAX-PLANCK'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='MSRC-12'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='OXFORD'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='PAMAP'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt

dataset='UCI-GAS'
echo $dataset
$compressor dataset/$dataset/$dataset.bin > res_lz_tz/$dataset.txt
