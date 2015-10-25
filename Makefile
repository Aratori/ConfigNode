CLIBS = -ljsoncpp -ltinyxml -lgtest -lgtest_main  -lpthread
CFLAGS = -std=c++11 -g

all: test

test: test.o ConfigNodeJson.o ConfigNodeXml.o ConfigNodeBase.o
	g++ $(CFLAGS) test.o ConfigNodeJson.o ConfigNodeXml.o ConfigNodeBase.o -o	Test $(CLIBS)
	 
main.o: main.cpp ConfigNodeJson.h ConfigNodeJson.cpp
	g++ $(CFLAGS)  -c main.cpp 
	
test.o: test.cpp ConfigNodeJson.h ConfigNodeJson.cpp ConfigNodeXml.h ConfigNodeXml.cpp ConfigNodeBase.h ConfigNodeBase.cpp
	g++ $(CFLAGS) -c test.cpp

ConfigNodeJson.o: ConfigNodeJson.h ConfigNodeJson.cpp ConfigNodeBase.h ConfigNodeBase.cpp
	g++ $(CFLAGS) -c ConfigNodeJson.cpp
	
ConfigNodeXml.o: ConfigNodeXml.h ConfigNodeXml.cpp ConfigNodeBase.h ConfigNodeBase.cpp
	g++ $(CFLAGS) -c ConfigNodeXml.cpp
	
ConfigNodeBase.o: ConfigNodeBase.h ConfigNodeBase.cpp
	g++ $(CFLAGS) -c ConfigNodeBase.cpp	
	
	
clean:
	rm -rf *.o Test
