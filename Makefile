CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -g

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

routewise: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): $(wildcard src/*.h)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o routewise
