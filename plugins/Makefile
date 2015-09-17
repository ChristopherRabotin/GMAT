# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
#  Copyright (c) 2002-2014 United States Government as represented by the
#  Administrator of The National Aeronautics and Space Administration.
#  All Other Rights Reserved.
#
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
	cd GeometricMeasurementPlugin; make all;
	cd EventLocatorPlugin; make all;
	cd ExtraPropagatorsPlugin; make all;
	cd NewParameterPlugin; make all;
	cd PolyhedronGravityPlugin; make all;
	cd ProductionPropagatorPlugin; make all;
	cd GmatFunctionPlugin; make all;
	cd PythonInterfacePlugin; make all;
	cd FormationPlugin; make all;
	cd SaveCommandPlugin; make all;
	cd StationPlugin; make all;
	cd DataInterfacePlugin; make all;

rebuild: 
	cd CInterfacePlugin; make rebuild;
	cd EphemPropagatorPlugin; make rebuild;
	cd EstimationPlugin; make rebuild;
	cd GeometricMeasurementPlugin; make rebuild;
	cd EventLocatorPlugin; make rebuild;
	cd ExtraPropagatorsPlugin; make rebuild;
	cd NewParameterPlugin; make rebuild;
	cd PolyhedronGravityPlugin; make rebuild;
	cd ProductionPropagatorPlugin; make rebuild;
	cd GmatFunctionPlugin; make rebuild;
	cd PythonInterfacePlugin; make rebuild;
	cd FormationPlugin; make rebuild;
	cd SaveCommandPlugin; make rebuild;
	cd StationPlugin; make rebuild;
	cd DataInterfacePlugin; make rebuild;

clean:
	cd CInterfacePlugin; make clean;
	cd EphemPropagatorPlugin; make clean;
	cd EstimationPlugin; make clean;
	cd GeometricMeasurementPlugin; make clean;
	cd EventLocatorPlugin; make clean;
	cd NewParameterPlugin; make clean;
	cd PolyhedronGravityPlugin; make clean;
	cd ProductionPropagatorPlugin; make clean;
	cd ExtraPropagatorsPlugin; make clean;
	cd GmatFunctionPlugin; make clean;
	cd PythonInterfacePlugin; make clean;
	cd FormationPlugin; make clean;
	cd SaveCommandPlugin; make clean;
	cd StationPlugin; make clean;
	cd DataInterfacePlugin; make clean;

endif

ifeq ($(PLATFORM), mac)

all: 
	cd CInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
#	cd MatlabInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);

rebuild: 
	cd CInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
#	cd MatlabInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);

clean:
	cd CInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
#	cd MatlabInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);

endif

ifeq ($(PLATFORM), windows)

all: 
	cd CInterfacePlugin; make all;
	cd EphemPropagatorPlugin; make all;
	cd EstimationPlugin; make all;
	cd GeometricMeasurementPlugin; make all;
	cd EventLocatorPlugin; make all;
	cd ExtraPropagatorsPlugin; make all;
	cd NewParameterPlugin; make all;
	cd PolyhedronGravityPlugin; make all;
	cd ProductionPropagatorPlugin; make all;
	cd GmatFunctionPlugin; make all;
	cd MatlabInterfacePlugin; make all;
	cd PythonInterfacePlugin; make all;
	cd FminconOptimizerPlugin; make all;
	cd FormationPlugin; make all;
	cd SaveCommandPlugin; make all;
	cd StationPlugin; make all;
	cd DataInterfacePlugin; make all;

rebuild: 
	cd CInterfacePlugin; make rebuild;
	cd EphemPropagatorPlugin; make rebuild;
	cd EstimationPlugin; make rebuild;
	cd GeometricMeasurementPlugin; make rebuild;
	cd EventLocatorPlugin; make rebuld;
	cd ExtraPropagatorsPlugin; make rebuild;
	cd NewParameterPlugin; make rebuild;
	cd PolyhedronGravityPlugin; make rebuild;
	cd ProductionPropagatorPlugin; make rebuild;
	cd GmatFunctionPlugin; make rebuild;
	cd MatlabInterfacePlugin; make rebuild;
	cd PythonInterfacePlugin; make rebuild;
	cd FminconOptimizerPlugin; make rebuild;
	cd FormationPlugin; make rebuild;
	cd SaveCommandPlugin; make rebuild;
	cd StationPlugin; make rebuild;
	cd DataInterfacePlugin; make rebuild;

clean:
	cd CInterfacePlugin; make clean;
	cd EphemPropagatorPlugin; make clean;
	cd EstimationPlugin; make clean;
	cd GeometricMeasurementPlugin; make clean;
	cd EventLocatorPlugin; make clean;
	cd ExtraPropagatorsPlugin; make clean;
	cd NewParameterPlugin; make clean;
	cd PolyhedronGravityPlugin; make clean;
	cd ProductionPropagatorPlugin; make clean;
	cd GmatFunctionPlugin; make clean;
	cd MatlabInterfacePlugin; make clean;
	cd PythonInterfacePlugin; make clean;
	cd FminconOptimizerPlugin; make clean;
	cd FormationPlugin; make clean;
	cd SaveCommandPlugin; make clean;
	cd StationPlugin; make clean;
	cd DataInterfacePlugin; make clean;

endif

ifeq ($(PLATFORM), linux)

all: 
	cd CInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make all BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make all BUILD_64BIT=$(BUILD_64BIT);

rebuild: 
	cd CInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make rebuild BUILD_64BIT=$(BUILD_64BIT);

clean:
	cd CInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EstimationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd GeometricMeasurementPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EphemPropagatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd EventLocatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd ExtraPropagatorsPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd NewParameterPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd PolyhedronGravityPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd ProductionPropagatorPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd GmatFunctionPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd MatlabInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd PythonInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd FormationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd SaveCommandPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd StationPlugin; make clean BUILD_64BIT=$(BUILD_64BIT);
	cd DataInterfacePlugin; make clean BUILD_64BIT=$(BUILD_64BIT);

endif
