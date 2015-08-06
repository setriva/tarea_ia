CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=
SOURCES=aux.cpp main.cpp maquina.cpp proceso.cpp servicio.cpp solucion.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tarea_ia

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
