CXXFLAGS += -std=c++11 -g -O3 -Wall

SRCS := $(shell ls [0-9][0-9][0-9]-*.cpp)
EXES := $(patsubst %.cpp,%,$(SRCS))

.PHONY : all clean

all : $(EXES)

$(EXES) : test.hpp

clean : 
	$(RM) $(EXES)
