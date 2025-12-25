CC = clang++
CPPFLAGS = -std=c++23 -Wall -Wextra

main: main.cpp
	$(CC) $(CPPFLAGS) -o main main.cpp

clean:
	rm -f main
