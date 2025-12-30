CXX       := clang++
CXXFLAGS  := -std=c++23 -Wall -Wextra -I. -I/opt/homebrew/include
LDFLAGS   := -L/opt/homebrew/lib

LIB_DIR   := my
TEST_DIR  := tests
BUILD_DIR := build
BIN_DIR   := bin

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
LIB_SRCS  := $(wildcard $(LIB_DIR)/*.hpp)

main: main.cpp $(LIB_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/main main.cpp $(LDFLAGS)

test: $(TEST_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) -o $(BIN_DIR)/run_tests $(LDFLAGS) -lCatch2 -lCatch2Main

clean:
	rm -rf $(BIN_DIR)
