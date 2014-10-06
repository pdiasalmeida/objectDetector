CC =	g++

CFLAGS =	-c -g -O0 -Wall `pkg-config --cflags opencv`

LDFLAGS =	`pkg-config --libs opencv`

SOURCES =	../../auxiliar/FilesHelper.cpp \
			cutPicture.cpp

OBJECTS =	$(SOURCES:.cpp=.o)

EXECUTABLE =	cutPicture

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):	$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ./thumbs/*.jpg
