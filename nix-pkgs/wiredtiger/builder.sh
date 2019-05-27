source $stdenv/setup

rsync -rltD $src/ ./

find . -type f -exec chmod +w {} \;
find . -type d -exec chmod +w {} \;

./autogen.sh

./configure \
    --prefix=$out \
    --enable-tcmalloc \
    --with-spinlock=gcc

make -j 16

make install
