TARGET := async-ip-sweep-cpp

CXX := g++
CXX_FLAGS := -Wall -O3
LD_FLAGS := -lpthread

.PHONY: all clean
all: $(TARGET)
clean: 
	rm $(TARGET)

$(TARGET): async-ip-sweep.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@ $(LD_FLAGS)

