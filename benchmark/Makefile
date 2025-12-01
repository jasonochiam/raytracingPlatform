CXX := g++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra

TARGET := benchmark
SRCS := main.cc
OBJS := $(SRCS:.cc=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET) > image.ppm

clean:
	rm -f $(OBJS) $(TARGET) image.ppm
