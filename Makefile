CLIBS = -ljsoncpp -ltinyxml -lgtest -lgtest_main  -lpthread
CFLAGS = -std=c++11 -g

all: test

test: test.o ConfigNode*.o
	g++ $(CFLAGS) test.o ConfigNodeJson.o ConfigNodeXml.o ConfigNodeBase.o -o	Test $(CLIBS)
	
test.o: test.cpp ConfigNode*.h ConfigNode*.cpp
	g++ $(CFLAGS) -c test.cpp

ConfigNodeJson.o: ConfigNode*.h ConfigNode*.cpp
	g++ $(CFLAGS) -c ConfigNodeJson.cpp
	
ConfigNodeXml.o: ConfigNode*.h ConfigNode*.cpp
	g++ $(CFLAGS) -c ConfigNodeXml.cpp
	
ConfigNodeBase.o: ConfigNodeBase.h ConfigNodeBase.cpp
	g++ $(CFLAGS) -c ConfigNodeBase.cpp	
	
	
clean:
	rm -rf *.o Test
