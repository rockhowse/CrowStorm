#simple make for compiling crow_storm

# compile the crow_storm application
all:
	g++ -o crow_storm crow_storm.cpp -std=c++14 -O0 -g -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lpthread -lcurl
	
debug:
	g++ -o crow_storm_d crow_storm.cpp -std=c++14 -g -O0 -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lpthread -lcurl
	
# clean up the compiled executible
clean:
	rm -f crow_storm
	rm -f crow_storm_d
