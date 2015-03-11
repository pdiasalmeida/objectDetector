CC =	g++

CFLAGS =	-c -g -O0 -Wall `pkg-config --cflags opencv`

LDFLAGS =	`pkg-config --libs opencv` -lpqxx

SOURCES =	auxiliar/FilesHelper.cpp \
			auxiliar/FileWriter.cpp \
			validators/SVMValidator.cpp \
			validators/ColorValidator.cpp \
			validators/HistValidator.cpp \
			classifier/TemplateClassifier.cpp \
			classifier/SVMClassifier.cpp \
			classifier/FixedClassifier.cpp \
			detectors/CascadeClassifierDetector.cpp \
			data/FileDataStore.cpp \
			data/PostgresDataStore.cpp \
			ObjectDetector.cpp

OBJECTS =	$(SOURCES:.cpp=.o)

EXECUTABLE =	detectObjects

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):	$(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
