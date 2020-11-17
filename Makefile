vpath %.hpp ./include
vpath %.cpp ./src

ifeq ($(debug),1)
	DEBUG = -D__DEBUG__ -g
else
	DEBUG = -g
endif

INCLUDE = ./include

CPPFLAGS = $(DEBUG) -I $(INCLUDE) -Wall -std=c++11 -lstdc++ -pthread

objects = test.o Server.o IPSet.o


test: $(objects)
	cc $(CPPFLAGS) -o test $(objects)

test.o : Server.hpp IPSet.hpp
Server.o : Server.hpp
IPSet.o : IPSet.hpp

.PHONY : clean
clean:
	rm test $(objects)


