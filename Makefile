#simple make for compiling crow_storm

# compile the crow_storm application
all:
	#g++ -o crow_storm crow_storm.cpp -std=c++14 -O3 -lboost_system -lboost_thread -lpthread -lcurl
	g++ -o crow_storm crow_storm.cpp -std=c++14 -g -O0 -lboost_system -lboost_thread -lpthread -lcurl
	
debug:
	g++ -o crow_storm_d crow_storm.cpp -std=c++14 -g -O0 -lboost_system -lboost_thread -lpthread -lcurl
	
# clean up the compiled executible
clean:
	rm -f crow_storm
	rm -f crow_storm_d
