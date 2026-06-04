CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET   = traffic_sim
SRCS     = main.cpp \
           src/Vehicle.cpp \
           src/Road.cpp \
           src/Intersection.cpp \
           src/TrafficLight.cpp \
           src/TrafficAccident.cpp \
           src/AccidentManager.cpp \
           src/CityMap.cpp \
           src/RoutePlanner.cpp \
           src/FileManager.cpp \
           src/Simulation.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	del /Q /F $(subst /,\,$(OBJS)) $(TARGET).exe 2>nul || true

run: all
	./$(TARGET)

.PHONY: all clean run
