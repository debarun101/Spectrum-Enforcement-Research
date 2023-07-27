CXX = g++
LDFLAGS =
CXXFLAGS =
CPPFLAGS =
MAKE = make

all: ex1cpp ex2cpp simul

lib/csim.cpp.a:
	(cd lib && $(MAKE))

ex1cpp: ex1cpp.cpp lib/csim.cpp.a
	$(CXX) -DCPP -Ilib $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o ex1cpp ex1cpp.cpp lib/csim.cpp.a -lm

ex2cpp: ex2cpp.cpp lib/csim.cpp.a
	$(CXX) -DCPP -Ilib $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o ex2cpp ex2cpp.cpp lib/csim.cpp.a -lm

simul: simul.cpp lib/csim.cpp.a
	$(CXX) -m32 -std=c++0x -g -DCPP -Ilib $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o simul simul.cpp lib/csim.cpp.a -lm

clean:
	rm -f ex1cpp ex2cpp simul
