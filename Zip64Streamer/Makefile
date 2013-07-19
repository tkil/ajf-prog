CXXFLAGS += -std=c++11

EXE  := Zip64StreamerTest
OBJS := Zip64StreamerTest.o Zip64Streamer.o Compat.o

$(EXE) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) -lz

Zip64Streamer.o : Zip64Streamer.cpp Zip64Streamer.hpp Compat.hpp

Compat.o : Compat.cpp Compat.hpp

Zip64StreamerTest.o : Zip64StreamerTest.cpp Zip64Streamer.hpp Compat.hpp

clean :
	$(RM) $(EXE) $(OBJS)
