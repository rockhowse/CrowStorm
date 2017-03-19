# CrowStorm
Simple crow based server for requesting and displaying 30 days worth of averages prices for a specific security.

## Dependencies:

### Server (crow + C++14 compatible compiler)
1. [boost (v1.58.0)](http://www.boost.org/)   	- C++ extension libraries
2. [cmake (v3.5.1)](https://cmake.org/)		- Flexible make configuration 
3. [crow](https://github.com/ipkn/crow)	- Simple/fast C++ HTTP server backend 
4. [openssl (v1.0.2g)](https://www.openssl.org/)	- encryption libraries used for HTTPS/SSL
5. [tmalloc (v4.x.x)](http://goog-perftools.sourceforge.net/doc/tcmalloc.html) - libraries optimized for thread based malloc
6. [libcurl (v7.47.0)](https://github.com/curl/curl) - command line utility for using standard web protocols (HTTP, FTP etc.)

### Client (HTML5 + Javascript + css)

1. [jquery (v3.1.1)](https://jquery.com/)  - Industry Standard Javascript library for dynamic DOM manipulation
2. [bootstrap (v4.0.0)](https://v4-alpha.getbootstrap.com/) - Flexible modern styled javascript and CSS layer which allows clean and scalable UI creation
3. [tether (v1.4.0](http://tether.io/)  - A client-side library to make absolutely positioned elements attach to elements in the page efficiently
4. [techanjs (v0.8.0)](http://techanjs.org/) - D3 based charting library that provides many capabilities
5. [d3.js (v4.7.3)](https://d3js.org/) - Javascript library used for many dynamic interfaces uese HTML, SVG and CSS4.

## Attributions

CrowStorm icons were created from the original artwork of [shadow-2016 on deviantart.com](http://shadow-2016.deviantart.com/art/Crow-631521440).

## Configure and test crow install

Tested on: x64 gcc 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.4)

### cmake (v3.5.1)
~~~~
sudo apt-get install cmake
~~~~

### tmalloc (v4.X)
~~~~
sudo apt-get install google-perftools
~~~~

Cmake expects the library to be named "libtcmalloc.so" exactly, pop in symlink:  
~~~~
sudo ln -s /usr/lib/libtcmalloc.so.4.2.6 /usr/lib/libtcmalloc.so
~~~~

### openssl (v1.0.2g)
~~~~
sudo apt-get install libssl-dev
~~~~

### boost (v1.58.0)
~~~~
sudo apt-get install libboost-all-dev
~~~~

### libcurl (v?)
~~~~
sudo apt-get install libcurl4-gnutls-dev
~~~~


### crow (v?)

*NOTE* crow and CrowStorm should be cloned into the same parent directory as CrowStorm relies on the relative path ../crow

~~~~
git clone https://github.com/ipkn/crow
~~~~

You should now have everything you need to compile the crow server/example/tests:

~~~~
cd crow
mkdir build
cd build
cmake .. 

-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found Tcmalloc: /usr/lib/libtcmalloc.so
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE  
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
make
...
[100%] Generating test.py
[100%] Built target template_test_copy
ctest
Test project /home/rockhowse/chupacabra/crow/build
    Start 1: crow_test
1/2 Test #1: crow_test ........................   Passed    0.07 sec
    Start 2: template_test
2/2 Test #2: template_test ....................   Passed    0.37 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.44 sec
~~~~

You should have been able to compile 100% and run the tests 100%. If this was NOT the case, make sure to consult the crow documentation until all dependencies are satisfied before moving on.

## Compile and test the crow_storm application

Assuming you are still located in the crow/build directory perform the following commands:

~~~~
cd ../../
git clone https://github.com/rockhowse/CrowStorm
cd CrowStorm
~~~~

### Example usage

~~~~
make clean

rm -f crow_storm
rm -f crow_storm_d

make
g++ -o crow_storm crow_storm.cpp -std=c++14 -O3 -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lpthread -lcurl
~~~~

If this process completes without errors, all the dependencies are comfigured correctly and you should be rdy to start up the crow_storm server!

#### Server Side
~~~~
./crow_storm 

(2017-03-19 03:07:51) [INFO    ] Crow/0.1 server is running, local port 18080
(2017-03-19 03:07:51) [DEBUG   ] Connection open, total 1, 0x2133ef0
~~~~

You should now be able to open up your browser and connect to your local CrowStorm server on port 18080.

* [http://localhost:18080](http://localhost:18080) - local CrowStorm server

The server downloads and caches the list of companies and symbols from Nasdaq's site on startup. You can validate these files were created by navigating to the data/ directory inside the CrowStorm project.

~~~~
cd data
ls -alh

total 868K
drwxrwxr-x 2 rockhowse rockhowse 4.0K Mar 18 22:07 .
drwxrwxr-x 6 rockhowse rockhowse 4.0K Mar 18 22:07 ..
-rw-rw-r-- 1 rockhowse rockhowse  46K Mar 18 22:07 amex.csv
-rw-rw-r-- 1 rockhowse rockhowse 417K Mar 18 22:07 nasdaq.csv
-rw-rw-r-- 1 rockhowse rockhowse 392K Mar 18 22:07 nyse.csv
~~~~

#### Client Side
![crow_storm](http://rockhowse.com/projects/CrowStorm/2017-03-15~crow_storm~v0.0.1.png)

