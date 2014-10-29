# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT plugins directory
# That is, we should have the directory structure:
#   ./CInterfacePlugin
#   ./DataInterfacePlugin
#   etc... other plugins
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# CInterface Plugin binary
if (PLUGIN_CINTERFACE)
  SUBDIRS(CInterfacePlugin/src/)
endif()

# ====================================================================
# CInterface Plugin binary
if (PLUGIN_DATAINTERFACE)
  SUBDIRS(DataInterfacePlugin/src/)
endif()

# ====================================================================
# EphemPropagator Plugin binary
if (PLUGIN_EPHEMPROPAGATOR)
  SUBDIRS(EphemPropagatorPlugin/src/)
endif()

# ====================================================================
# Estimation Plugin binary
if (PLUGIN_ESTIMATION)
  SUBDIRS(EstimationPlugin/src/)
endif()

# ====================================================================
# EventLocator Plugin binary
if (PLUGIN_EVENTLOCATOR)
  SUBDIRS(EventLocatorPlugin/src/)
endif()

# ====================================================================
# ExtraPropagators Plugin binary
if (PLUGIN_EXTRAPROPAGATORS)
  SUBDIRS(ExtraPropagatorsPlugin/src/)
endif()

# ====================================================================
# FminconOptimizer Plugin binary
if (PLUGIN_FMINCONOPTIMIZER)
  SUBDIRS(FminconOptimizerPlugin/src/)
endif()

# ====================================================================
# Formation Plugin binary
if (PLUGIN_FORMATION)
  SUBDIRS(FormationPlugin/src/)
endif()

# ====================================================================
# GeometricMeasurement Plugin binary
if (PLUGIN_GEOMETRICMEASUREMENT)
  SUBDIRS(GeometricMeasurementPlugin/src/)
endif()

# ====================================================================
# GmatFunction Plugin binary
if (PLUGIN_GMATFUNCTION)
  SUBDIRS(GmatFunctionPlugin/src/)
endif()

# ====================================================================
# MatlabInterface Plugin binary
if (PLUGIN_MATLABINTERFACE)
  SUBDIRS(MatlabInterfacePlugin/src/)
endif()

# ====================================================================
# ProductionPropagators Plugin binary
if (PLUGIN_PRODUCTIONPROPAGATORS)
  SUBDIRS(ProductionPropagatorPlugin/src/)
endif()

# ====================================================================
# SaveCommand Plugin binary
if (PLUGIN_SAVECOMMAND)
  SUBDIRS(SaveCommandPlugin/src/)
endif()

# ====================================================================
# Station Plugin binary
if (PLUGIN_STATION)
  SUBDIRS(StationPlugin/src/)
endif()
