Compression library based on zlib with QIODevice interface.

Works like all QIODevice objects. Compresses when write to device and decompresses when read from
device.

Not QIODevice public members:

void setDevice(QIODevice *device) - sets device to write/read compressed data.

QIODevice* device() const - gets device to write/read compressed data.

void setCompressLevel(int level) - sets compress level 0-9, 0 - no compress, 1 - best speed, 9 best
compression.

int compressLevel() const - gets compress level.

void setCompressFormat(ZCompressor::CompressFormat format) - sets compress format,
ZCompressor::CompressFormat can be ZlibFormat, GzipFormat, RawDeflateFormat.

ZCompressor::CompressFormat compressFormat() const - gets compress format.

int state() const - get compression state Z_OK, ZERRNO etc. More info in zlib documentation.

unsigned long totalIn() const - total number of input bytes to compress so far.

unsigned long totalOut() const - total number of compressed bytes output so far.

Not QIODevice public static members:

int def(QIODevice *src, QIODevice *dest, int level, ZCompressor::CompressFormat format) - compress
data from src to dest at a time, with a certain compress level and compress format.

int def(const QByteArray &src, QIODevice *dest, int level, ZCompressor::CompressFormat format) -
compress data from byte array src to dest at a time, with a certain compress level and compress
format.

int inf(QIODevice *src, QIODevice *dest, ZCompressor::CompressFormat format) - decompress data from
src to dest at a time, with certain compress format.

Building in Linux:
Install zlib dev package. In Ubuntu zlib1g-dev.
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make

Building in Windows with MSVC 2017:
Download or build zlib.
mkdir build
cd build
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path Qt MSVC>
-DZLIB_DIR=<path to zlib lib file> -DZLIB_INCLUDE=<path to zlib include files> ..
nmake

cli program will be in bin folder, libraries will be in lib folder.