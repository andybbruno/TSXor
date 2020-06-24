# LZ-XOR
[Alpha Version] LZ-XOR: A Novel Time Series Compression Algorithm

Fully developed at ISTI CNR - [HPC Lab](http://hpc.isti.cnr.it) (PISA)

##### Table of contents
* [Building the code](#building-the-code)
* [Input data format](#input-data-format)
* [Tests](#tests)
* [Benchmarks](#benchmarks)


Builiding the code
-----------------
We tested the code on MacOS 10.15.4 using `clang` version 11.0.3. 

No dependencies are needed.

Just execute:

    make all


Input data format
-----------------
The algorithm can process any `.csv` file containing numbers only.
You need first to convert the `.csv` into `.bin` using the `csv_to_bin` utility as follows:

    cd util

    ./csv_to_bin.o path/to/MY_DATASET.csv

Please note: the first column will be interpret as the timestamp, the rest will be interpret as values.



Tests
-----------------
### Compression

To run a compression test of a `.bin` file, execute the following commands:

    cd test

    ./compression.o path/to/MY_DATASET.bin

This will produce a file called ``compressed_data.lzx``

### Decompression

To decompress the file ``compressed_data.lzx``, run the command:

    ./decompression.o

Benchmarks
-----------------
These are some comparisons between LZ-XOR with [Gorilla](https://www.vldb.org/pvldb/vol8/p1816-teller.pdf) by Facebook.

### Compression Speeds (MB/s)

|                      |  FPC  | Gorilla | LZ-XOR |
|:--------------------:|:-----:|:-------:|:------:|
|        AMPds2        | 126,9 |  **381,8**  |  77,4  |
|       Bar Crawl      | 232,4 |  **325,8**  |  35,2  |
|      Max-Planck      | 156,1 |  **484,8**  |  64,2  |
|        Kinect        |  82,2 |  **399,8**  |  23,5  |
|      Oxford-Man      |  63,7 |  **348,5**  |  19,8  |
|         PAMAP        |  76,6 |  **351,5**  |  56,0  |
| UCI Gas Sensor Array | 112,9 |  **349,4**  |  28,6  |


### Decompression Speeds (MB/s)

|                      |   FPC   | Gorilla |  LZ-XOR |
|:--------------------:|:-------:|:-------:|:-------:|
|        AMPds2        |  210,3  |  499,1  | **1.068,1** |
|       Bar Crawl      |  293,0  |  352,1  |  **572,9**  |
|      Max-Planck      |  229,7  |  646,3  |  **870,5**  |
|        Kinect        |  150,2  |  502,6  |  **527,3** |
|      Oxford-Man      |  108,7  |  436,6  |  **534,0**  |
|         PAMAP        | **1.635,4** |  391,7  |  841,8  |
| UCI Gas Sensor Array |  117,8  |  443,9  |  **604,2**  |


### Compression Ratios

|                      |   FPC  | Gorilla | LZ-XOR |
|:--------------------:|:------:|:-------:|:------:|
|        AMPds2        | 1.375x |  2.034x | **6.391x** |
|       Bar Crawl      | 1.461x |  1.443x | **2.358x** |
|      Max-Planck      | 2.707x |  2.968x | **4.837x** |
|        Kinect        | **1.478x** |  1.408x | 1.367x |
|      Oxford-Man      | 1.233x |  1.275x | **1.299x** |
|         PAMAP        | 1.364x |  1.380x | **4.848x** |
| UCI Gas Sensor Array | 1.338x |  1.228x | **3.499x** |