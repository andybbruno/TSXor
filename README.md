# TSXor
[Alpha Version] TSXor: A Novel Time Series Compression Algorithm

Fully developed at ISTI CNR - [HPC Lab](http://hpc.isti.cnr.it) (Pisa)

##### Table of contents
* [Building the code](#building-the-code)
* [Input data format](#input-data-format)
* [Tests](#tests)
* [Benchmarks](#benchmarks)
* [Notes](#notes)

Builiding the code
-----------------
The code has been tested both on Linux and MacOS.

No dependencies are needed.

Just clone this repo and execute:

    make all


Input data format
-----------------
The algorithm can process any `.csv` file containing numbers only.
You need first to convert the `.csv` into a `.bin` file using the `csv_to_bin` utility as follows:

    cd util

    ./csv_to_bin.o path/to/MY_DATASET.csv

Please note: the first column will be interpret as the timestamp, the rest will be interpret as values.



Tests
-----------------
### Compression

To run a compression test of a `.bin` file, execute the following commands:

    cd test

    ./compression.o path/to/MY_DATASET.bin

This will produce a file called ``compressed_data.tsx``

### Decompression

To decompress the file ``compressed_data.tsx``, run the command:

    ./decompression.o

Benchmarks
-----------------
The following tables show the comparison between TSXor with [Gorilla](https://www.vldb.org/pvldb/vol8/p1816-teller.pdf) by Facebook and [FPC](https://ieeexplore.ieee.org/document/4589203) by Burtscher and Ratanaworabhan. The experiments were run on an Ubuntu 18.04 machine with Intel i7-7700 CPU @ 3.60GHz.

### Compression Speeds (MB/s)

|                      |  FPC  | Gorilla | TSXor |
|:--------------------:|:-----:|:-------:|:------:|
|        AMPds2        | 339,28 | **703,72** | 66,59 |
|       Bar Crawl      | 423,71 | **466,49** | 28,74 |
|      Max-Planck      | 313,40 | **870,58** | 51,74 |
|        Kinect        | 166,28 | **696,10** | 17,14 |
|      Oxford-Man      | 170,27 | **630,33** | 15,43 |
|         PAMAP        | 181,59 | **521,41** | 45,05 |
| UCI Gas Sensor Array | 286,94 | **654,32** | 21,93 |

### Decompression Speeds (MB/s)

|                      |   FPC   | Gorilla |  TSXor |
|:--------------------:|:-------:|:-------:|:-------:|
|        AMPds2        | 411,29 | 666,52 | **1173,65** |
|       Bar Crawl      | 436,12 | 447,42 | **709,68**  |
|      Max-Planck      | 355,30 | 858,68 | **1057,00** |
|        Kinect        | 287,18 | 635,74 | **665,47**  |
|      Oxford-Man      | 221,80 | 573,67 | **604,54**  |
|         PAMAP        | 223,86 | 487,41 | **949,28**  |
| UCI Gas Sensor Array | 454,91 | 578,41 | **642,40**  |


### Compression Ratios

|                      |   FPC  | Gorilla | TSXor |
|:--------------------:|:------:|:-------:|:------:|
|        AMPds2        | 1,10x | 2,03x | **6,39x** |
|       Bar Crawl      | 1,20x | 1,44x | **2,36x** |
|      Max-Planck      | 1,06x | 2,97x | **4,84x** |
|        Kinect        | 1,09x | **1,41x** | 1,37x |
|      Oxford-Man      | 1,06x | 1,28x | **1,30x** |
|         PAMAP        | 1,01x | 1,38x | **4,85x** |
| UCI Gas Sensor Array | 1,19x | 1,23x | **3,50x** |


Notes
-----------------
This is a preliminary version. Further details will be published in the next weeks.
