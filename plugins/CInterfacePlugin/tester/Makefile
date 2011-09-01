# Makefile for the C-Interface test program
include ../src/CInterfaceEnv.mk

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
	$(CC) $(CFLAGS) $(HEADERS) -DLINUX $(DEBUG_FLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(PLATFORM_LINK_FLAGS) $(DEBUG_FLAGS)
	mv $(TARGET) $(GMAT_BIN_LOCATION)
