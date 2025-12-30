CXX       := clang++
CXXFLAGS  := -std=c++23 -Wall -Wextra -I. -I/opt/homebrew/include
LDFLAGS   := -L/opt/homebrew/lib  -lCatch2 -lCatch2Main

LIB_DIR   := my
TEST_DIR  := tests
BUILD_DIR := build
BIN_DIR   := bin

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
LIB_SRCS  := $(wildcard $(LIB_DIR)/*.hpp)

TEST_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

main: main.cpp $(LIB_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/main main.cpp

test: $(BIN_DIR)/run_tests

$(BIN_DIR)/run_tests: $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(TEST_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
