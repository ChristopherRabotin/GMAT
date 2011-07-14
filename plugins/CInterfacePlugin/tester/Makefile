# Makefile for the C-Interface test program

GMAT_TARGET_DIRECTORY = /Users/djc/TS_Code/GmatDevelopment/application/bin64

TARGET = CInterfaceTester

CC = gcc

HEADERS = -I../src/include
CFLAGS  = -O3 -fno-strict-aliasing $(WX_28_DEFINES) -fPIC -Wall

OBJECTS = CInterfaceTester.o

all: prep $(TARGET)

prep:
	rm -rf $(TARGET)	
	rm -rf *.o

$(OBJECTS): %.o: %.c %.h
	$(CC) $(CFLAGS) $(HEADERS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)
	mv $(TARGET) $(GMAT_TARGET_DIRECTORY)
