CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

inventory: src/main.cpp src/common.h
	$(CXX) $(CXXFLAGS) -o inventory src/main.cpp

run: inventory
	./inventory

clean:
	rm -f inventory
