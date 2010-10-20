include ../Makefile.inc

SRC	= gamesession.cpp

OBJS = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

all: $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -MMD -c -I.. $< -o $*.o

clean:
	rm -f *.o *.d

-include $(DEPS)
