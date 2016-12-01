
CC=g++
CFLAGS=-c -Wall
LDFLAGS=

SOURCES=main.cpp moon.cpp planet.cpp tga.cpp camera.cpp solarsystem.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=moonPhases

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -framework OpenGL -framework GLUT

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -framework OpenGL -framework GLUT

clean:
	rm *.o *.out $(EXECUTABLE)
