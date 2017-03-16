#simple make for compiling crow_storm

# compile the crow_storm application
crow_storm:
	g++ -o crow_storm crow_storm.cpp -std=c++14 -O3 -lboost_system -lboost_thread -lpthread

# clean up the compiled executible
clean:
	rm -f crow_storm
