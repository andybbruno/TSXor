# LZ-XOR
[Alpha Version] LZ-XOR: A Novel Time Series Compression Algorithm

Fully developed at ISTI CNR - [HPC Lab](http://hpc.isti.cnr.it) (PISA)

##### Table of contents
* [Building the code](#building-the-code)
* [Input data format](#input-data-format)
* [Tests](#tests)


Builiding the code
-----------------
We tested the code on MacOS 10.15.4 using `clang` version 11.0.3. 

No dependencies are needed.

Just execute
``make all``


Input data format
-----------------
The algorithm can process any `.csv` file containing numbers only.
You need first to convert the `.csv` into `.bin` using the `csv_to_bin` utility as follows:

``cd util``

``./csv_to_bin.o path/to/MY_DATASET.csv``

Please note: the first column will be interpret as the timestamp, the rest will be interpret as values.



Tests
-----------------
### Compression

To run a compression test of a `.bin` file, execute the following commands:

``cd test``

``./compression.o path/to/MY_DATASET.bin``

This will produce a file called ``compressed_data.lzx``

### Decompression

To decompress the file ``compressed_data.lzx``, run the command:

 ``./decompression.o``
