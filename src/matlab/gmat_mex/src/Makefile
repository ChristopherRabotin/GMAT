###############################################################################
# Project:    GMAT
# Module:     Makefile.mac for MATLAB client
# Created:    Joey Gurgnaus 
# Date:       2005/10/26
###############################################################################
#
# Program to make (library)
PROJECT_ROOT = ../../..
TOP_DIR = ..
LIBDIR = $(TOP_DIR)/lib
LIB = $(LIBDIR)/libMatlabClient.a
#
#
GUI_INCS = -I$(PROJECT_ROOT)/gui/include -I$(PROJECT_ROOT)/gui/include/bitmaps \
	-I.
#
# Source file
SRC = MatlabClient.cpp MatlabConnection.cpp

# Object modules for library
OBJ = $(SRC:.cpp=.o)
#
CXXFLAGS = $(GUI_INCS) -O2 `$(WX_HOME)/wx-config --cxxflags` -dynamic -Wall
#
CPP = g++ $(CXXFLAGS)
#
# Default suffix rule
.SUFFIXES: .o .cpp
#
###############################################################################
# Target definitions
#
all: $(LIB)

$(LIB): $(OBJ)
	@if [ ! -x $(LIBDIR) ]; then mkdir $(LIBDIR); fi
	ar cr $@ $(OBJ)
	@ranlib $@
	@chmod -R 755 $(LIBDIR)

.cpp.o:
	$(CPP) -c $<

clean:
	rm -f $(OBJ) core

bare: clean
	rm -rf $(LIB)
