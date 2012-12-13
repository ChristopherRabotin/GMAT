# $Id$
# Root level all plugins makefile 
#
# This file calls make on all plugins folder.

BUILD_64BIT = 0

ifeq ($(PLATFORM), )
PLATFORM = windows
endif

ifeq ($(PLATFORM), nomatlab)

all: 
	cd CInterfacePlugin; make all;
	cd EphemPropagatorPlugin; make all;
	cd EstimationPlugin; make all;
	cd EventLocatorPlugin; make all;
	cd ExtraPropagatorsPlugin; make all;
	cd GmatFunctionPlugin; make all;

rebuild: 
	cd CInterfacePlugin; make rebuild;
	cd EphemPropagatorPlugin; make rebuild;
	cd EstimationPlugin; make rebuild;
	cd EventLocatorPlugin; make rebuild;
	cd ExtraPropagatorsPlugin; make rebuild;
	cd GmatFunctionPlugin; make rebuild;

clean:
	cd CInterfacePlugin; make clean;
	cd EphemPropagatorPlugin; make clean;
	cd EstimationPlugin; make clean;
	cd EventLocatorPlugin; make clean;
	cd ExtraPropagatorsPlugin; make clean;
	cd GmatFunctionPlugin; make clean;

else

ifeq ($(PLATFORM), windows)

all: 
	cd CInterfacePlugin; make all;
	cd EphemPropagatorPlugin; make all;
	cd EstimationPlugin; make all;
	cd EventLocatorPlugin; make all;
	cd ExtraPropagatorsPlugin; make all;
	cd FminconOptimizerPlugin; make all;
	cd GmatFunctionPlugin; make all;
	cd MatlabInterfacePlugin; make all;

rebuild: 
	cd CInterfacePlugin; make rebuild;
	cd EphemPropagatorPlugin; make rebuild;
	cd EstimationPlugin; make rebuild;
	cd EventLocatorPlugin; make rebuld;
	cd ExtraPropagatorsPlugin; make rebuild;
	cd FminconOptimizerPlugin; make rebuild;
	cd GmatFunctionPlugin; make rebuild;
	cd MatlabInterfacePlugin; make rebuild;

clean:
	cd CInterfacePlugin; make clean;
	cd EphemPropagatorPlugin; make clean;
	cd EstimationPlugin; make clean;
	cd EventLocatorPlugin; make clean;
	cd ExtraPropagatorsPlugin; make clean;
	cd FminconOptimizerPlugin; make clean;
	cd GmatFunctionPlugin; make clean;
	cd MatlabInterfacePlugin; make clean;

else

all: 
	cd CInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);

rebuild: 
	cd CInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);

clean:
	cd CInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);

endif

endif