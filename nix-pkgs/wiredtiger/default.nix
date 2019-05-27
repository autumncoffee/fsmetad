{ cmake, stdenv, fetchzip, pkgs, rsync, autoconf, libtool, automake
}:

stdenv.mkDerivation rec {
  name = "wiredtiger-${version}";
  version = "3.2.0";

  builder = ./builder.sh;

  src = fetchzip {
    url = "https://github.com/wiredtiger/wiredtiger/releases/download/${version}/wiredtiger-${version}.tar.bz2";
    sha256 = "1mfiirsb36r5451zns2cvwsvq83g5khl9cjfhh62qwlshzv9hhfy";
  };

  nativeBuildInputs = [ rsync ];
  buildInputs = [
    cmake
    autoconf
    libtool
    automake
    pkgs.gperftools
    #openssl
    #zlib
  ]
    #++ stdenv.lib.optional stdenv.isDarwin pkgs.darwin.apple_sdk.frameworks.CoreFoundation
    #++ stdenv.lib.optional stdenv.isDarwin pkgs.darwin.apple_sdk.frameworks.Security
  ;
  #propagatedBuildInputs = [ snappy ];

  enableParallelBuilding = true;

  meta = with stdenv.lib; {
    description = "WiredTiger";
    homepage = https://github.com/wiredtiger/wiredtiger;
    license = licenses.asl20;
    platforms = platforms.all;
  };
}
