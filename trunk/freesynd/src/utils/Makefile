include ../Makefile.inc

SRC	= log.cpp dernc.cpp file.cpp

OBJS = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

all: $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -I.. -MMD -c $< -o $*.o

clean:
	rm -f *.o *.d

-include $(DEPS)
