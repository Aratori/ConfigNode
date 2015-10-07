CLIBS = -ljsoncpp
CFLAGS = -std=c++11 -g

all: main

main: main.o ConfigNodeJson.o
	g++ $(CFLAGS)  main.o -o Main $(CLIBS)
	 
main.o: main.cpp ConfigNodeJson.h ConfigNodeJson_impl.h
	g++ $(CFLAGS)  -c main.cpp 

ConfigNodeJson.o: ConfigNodeJson.h ConfigNodeJson_impl.h
	g++ $(CFLAGS) -c ConfigNodeJson.h ConfigNodeJson_impl.h
	
clean:
	rm -rf *.o Main
