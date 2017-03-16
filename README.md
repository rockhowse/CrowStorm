# CrowStorm
Simple crow based server for requesting and displaying 30 days worth of averages prices for a specific security.

## Dependencies:

1. [boost](http://www.boost.org/)   	- C++ extension libraries
2. [cmake](https://cmake.org/)		- Flexible make configuration 
3. [crow](https://github.com/ipkn/crow)	- Simple/fast C++ HTTP server backend 
4. [openSSL](https://www.openssl.org/)	- encryption libraries used for HTTPS/SSL
5. [tmalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html) - librares optimized for thread based malloc

## Installation

Tested on x64 Ubuntu 16:04 LTS:

### cmake (v3.5.1)
~~~~
sudo -apt -get install cmake
~~~~

### tmalloc (v4.X)
~~~~
sudo apt-get install google-perftools
~~~~

Cmake expects the library to be named "libtcmalloc.so" exactly, pop in symlink:  
~~~~
sudo ln -s /usr/lib/libtcmalloc.so.4.2.6 /usr/lib/libtcmalloc.so
~~~~

### openssl (v1.0.2g-1)
~~~~
sudo apt-get install libssl-dev
~~~~

### boost (v1.58.0)
~~~~
sudo apt-get install libboost-all-dev
~~~~

### crow (v?)
~~~~
sudo clone https://github.com/ipkn/crow
cd crow
mkdir build
cd build
cmake .. 
~~~~

Should result in something similar to this:

~~~~
-- Found Tcmalloc: /usr/lib/libtcmalloc.so
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libssl.so;/usr/lib/x86_64-linux-gnu/libcrypto.so (found version "1.0.2g") 
-- No build type selected, default to Release
-- Boost version: 1.58.0
-- Found the following Boost libraries:
--   system
--   thread
--   chrono
--   date_time
--   atomic
-- Configuring done
-- Generating done
-- Build files have been written to: /home/rockhowse/chupacabra/crow/build
~~~~


