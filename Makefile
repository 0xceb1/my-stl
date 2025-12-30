CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra
INCLUDES = -I. -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib

main: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o main main.cpp $(LDFLAGS)

clean:
	rm -f main
