###############################################################################
# Project:    GMAT
# Module:     Makefile.mac for MATLAB client
# Created:    Joey Gurgnaus 
# Date:       2005/10/26
###############################################################################
#
# Program to make (library)
PROJECT_ROOT  = ../../..
TOP_DIR       = ..
LIBDIR        = $(TOP_DIR)/lib
INCDIR        = $(TOP_DIR)/src
INSTALLED_LIB = $(LIBDIR)/libMatlabClient.a
EXECUTABLE    = TestDriver
#
#
INC_FLAG = -I$(PROJECT_ROOT)/gui/include -I$(PROJECT_ROOT)/gui/include/bitmaps \
	-I$(INCDIR) -I.
#
# Source file
SRC = $(EXECUTABLE).cpp

# LIB need to compile the EXECUTABLE
LIB     = -L$(LIBDIR)
# How they are invoked on the compile  line
LIBS    = `$(WX_HOME)/wx-config --libs` -lMatlabClient
#LIBS    = `$(WX_HOME)/wx-config --libs --gl-libs` -lMatlabClient

# Object modules for library
OBJ = $(SRC:.cpp=.o)
#
CXXFLAGS = $(INC_FLAG) -O2 `$(WX_HOME)/wx-config --cxxflags` -dynamic -Wall
#
CPP = g++
#
# Default suffix rule
.SUFFIXES: .o .cpp
#
###############################################################################
# Target definitions
#
###############################################################################
# Target defiitions
#
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ) $(INSTALLED_LIB)
	$(CPP) $(OBJ) $(LIB) $(LIBS) -o $@
	@chmod -R 755 $@ 
        
.cpp.o:
	$(CPP) $(CXXFLAGS) -c $<

clean:  
	rm -f $(OBJ) core

bare: clean
	rm -f $(EXECUTABLE) $(BINDIR)/$(EXECUTABLE)
