# CrowStorm
Simple crow based server for requesting and displaying 30 days worth of averages prices for a specific security.

## Dependencies:

### Server (crow + C++)
1. [boost](http://www.boost.org/)   	- C++ extension libraries
2. [cmake](https://cmake.org/)		- Flexible make configuration 
3. [crow](https://github.com/ipkn/crow)	- Simple/fast C++ HTTP server backend 
4. [openssl](https://www.openssl.org/)	- encryption libraries used for HTTPS/SSL
5. [tmalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html) - libraries optimized for thread based malloc
6. [libcurl](https://github.com/curl/curl) - command line utility for using standard web protocols (HTTP, FTP etc.)

### Client (HTML5 + Javascript + css)

1. [jquery](https://jquery.com/)  - Industry Standard Javascript library for dynamic DOM manipulation
2. [bootstrap](https://v4-alpha.getbootstrap.com/) - Flexible modern styled javascript and CSS layer which allows clean and scalable UI creation
3. [tether](http://tether.io/)  - A client-side library to make absolutely positioned elements attach to elements in the page efficiently

## Attributions

CrowStorm icons were created from the original artwork of [shadow-2016 on deviantart.com](http://shadow-2016.deviantart.com/art/Crow-631521440).

## Configure and test crow install

Tested on x64 Ubuntu 16.04 LTS:

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
You should now have everything you need to compile the crow server/example/tests:

~~~~
sudo clone https://github.com/ipkn/crow
cd crow
mkdir build
cd build
cmake .. 
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
make
ctest
~~~~

At this point you should be able to compile 100% and run the tests 100%

## Compile and test the crow_storm application

### Example usage

~~~~
rockhowse@cubacious-linux:~/chupacabra/CrowStorm$ make clean
rm -f crow_storm
rockhowse@cubacious-linux:~/chupacabra/CrowStorm$ make
g++ -o crow_storm crow_storm.cpp -std=c++14 -O3 -lboost_system -lboost_thread -lpthread
rockhowse@cubacious-linux:~/chupacabra/CrowStorm$ ./crow_storm
~~~~

#### Server Side
~~~~
rockhowse@cubacious-linux:~/chupacabra/CrowStorm$ ./crow_storm
(2017-03-16 02:02:32) [INFO    ] Crow/0.1 server is running, local port 18080
(2017-03-16 02:02:32) [DEBUG   ] Connection open, total 1, 0x11acd90
(2017-03-16 02:02:54) [DEBUG   ] 0x11acd90 timer cancelled: 0 0
(2017-03-16 02:02:54) [DEBUG   ] timer add inside: 0x7ff124d3b4b0 0
(2017-03-16 02:02:54) [DEBUG   ] 0x11acd90 timer added: 0x7ff124d3b4b0 0
(2017-03-16 02:02:54) [DEBUG   ] Connection open, total 2, 0x7ff110000960
(2017-03-16 02:02:54) [DEBUG   ] 0x11acd90 timer cancelled: 0x7ff124d3b4b0 0
(2017-03-16 02:02:54) [INFO    ] Request: 127.0.0.1:47674 0x11acd90 HTTP/1.1 GET /
(2017-03-16 02:02:54) [DEBUG   ] Matched rule '/' 1 / 2
(2017-03-16 02:02:54) [INFO    ] Response: 0x11acd90 / 200 0
(2017-03-16 02:02:54) [DEBUG   ] 0x11acd90 timer cancelled: 0 0
(2017-03-16 02:02:54) [DEBUG   ] timer add inside: 0x7ff124d3b4b0 1
(2017-03-16 02:02:54) [DEBUG   ] 0x11acd90 timer added: 0x7ff124d3b4b0 1
(2017-03-16 02:02:59) [DEBUG   ] timer call: 0x7ff124d3b4b0 1
(2017-03-16 02:02:59) [DEBUG   ] 0x11acd90 timer cancelled: 0x7ff124d3b4b0 1
(2017-03-16 02:02:59) [DEBUG   ] 0x11acd90 from read(1)
(2017-03-16 02:02:59) [DEBUG   ] 0x11acd90 is_reading 0 is_writing 0
(2017-03-16 02:02:59) [DEBUG   ] 0x11acd90 delete (idle)
(2017-03-16 02:02:59) [DEBUG   ] 0x11acd90 timer cancelled: 0 1
(2017-03-16 02:02:59) [DEBUG   ] Connection closed, total 1, 0x11acd90
~~~~

#### Client Side
![crow_storm](http://rockhowse.com/projects/CrowStorm/2017-03-15~crow_storm~v0.0.1.png)

