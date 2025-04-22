CC = gcc

CFLAGS = -Wall -g -O3 -fopenmp

TARGET = lcs

SOURCES = $(wildcard *.c)

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)