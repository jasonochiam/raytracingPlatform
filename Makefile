CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
LDFLAGS := -lSDL2

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
TARGET := raytracer

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo "Running $(TARGET)..."
	@./$(TARGET)

debug: CXXFLAGS += -g -O0
debug: clean all

clean:
	rm -f $(OBJS) $(TARGET) output1.ppm
