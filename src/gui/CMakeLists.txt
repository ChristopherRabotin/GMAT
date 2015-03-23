# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GUI
# This file must be installed in the src/gui directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified For CMake Overhaul By: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
# 

MESSAGE("==============================")
MESSAGE("GMAT GUI setup " ${VERSION})

# ====================================================================
# Major error checks
if(NOT wxWidgets_FOUND)
  MESSAGE(WARNING "Skipping due to missing wxWidgets")
  RETURN()
endif()

# ====================================================================
SET(TargetName GmatGUI)

# ====================================================================
# SPICE specifics
INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")
SET(SPICE_SRCS "spacecraft/SpicePanel.cpp")
ADD_DEFINITIONS("-D__USE_SPICE__")

# ====================================================================
# wxWidgets specifics
INCLUDE(${wxWidgets_USE_FILE})
LINK_LIBRARIES(${wxWidgets_LIBRARIES})

# ====================================================================
# Link libraries
LINK_LIBRARIES(GmatBase)

# Find OpenGL
FIND_PACKAGE(OpenGL)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
LINK_LIBRARIES(${OPENGL_LIBRARIES})

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-D__USE_STC_EDITOR__")

# ====================================================================
# list of directories containing source/header files
SET(GUI_DIRS
  app
  asset
  burn
  command
  controllogic
  coordsystem
  event
  forcemodel
  foundation
  function
  hardware
  include
  include/bitmaps
  include/il
  mission
  output
  propagator
  rendering
  solarsys
  solver
  spacecraft
  subscriber
  view
  )

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GUI_DIRS})
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})

# ====================================================================
# source files
SET(GUI_SRCS 
    app/AboutDialog.cpp
    app/GmatApp.cpp
    app/GmatAppData.cpp
    app/CompareFilesDialog.cpp
    app/CompareTextDialog.cpp
    app/GmatConnection.cpp
    app/GmatMainFrame.cpp
    app/GmatMenuBar.cpp
    app/GmatToolBar.cpp
    app/GuiInterpreter.cpp
    app/GuiMessageReceiver.cpp
    app/GmatNotebook.cpp
    app/GmatServer.cpp
    app/GmatTreeItemData.cpp
    app/GuiPlotReceiver.cpp
    app/InteractiveMatlabDialog.cpp
    app/MissionTreeItemData.cpp
    app/RunScriptFolderDialog.cpp
    app/ResourceTree.cpp
    app/ScriptPanel.cpp
    app/SetPathDialog.cpp
    app/TextEphemFileDialog.cpp
    app/WelcomePanel.cpp
    asset/GroundStationPanel.cpp
    burn/FiniteBurnSetupPanel.cpp
    burn/ImpulsiveBurnSetupPanel.cpp
    command/AchievePanel.cpp
    command/AssignmentPanel.cpp
    command/BeginFiniteBurnPanel.cpp
    command/CallFunctionPanel.cpp
    command/EndFiniteBurnPanel.cpp
    command/GmatCommandPanel.cpp
    command/ManageObjectPanel.cpp
    command/ManeuverPanel.cpp
    command/MinimizePanel.cpp
    command/NonlinearConstraintPanel.cpp
    command/OptimizePanel.cpp
    command/PropagatePanel.cpp
    command/ReportPanel.cpp
    command/ScriptEventPanel.cpp
    command/TargetPanel.cpp
    command/TogglePanel.cpp
    command/VaryPanel.cpp
    controllogic/ForPanel.cpp
    controllogic/ConditionPanel.cpp
    coordsystem/CoordPanel.cpp
    coordsystem/CoordSysCreateDialog.cpp
    coordsystem/CoordSystemConfigPanel.cpp
    forcemodel/DragInputsDialog.cpp
    foundation/ArraySetupDialog.cpp
    foundation/ArraySetupPanel.cpp
    foundation/GmatColorPanel.cpp
    foundation/GmatDialog.cpp
    foundation/GmatMdiChildFrame.cpp
    foundation/GmatPanel.cpp
    foundation/GmatBaseSetupPanel.cpp
    foundation/GmatSavePanel.cpp
    foundation/GmatStaticBoxSizer.cpp
    foundation/GuiItemManager.cpp
    foundation/MultiPathSetupPanel.cpp
    foundation/ParameterCreateDialog.cpp
    foundation/ParameterSelectDialog.cpp
    foundation/ParameterSetupPanel.cpp
    foundation/ShowScriptDialog.cpp
    foundation/ShowSummaryDialog.cpp
    foundation/SinglePathSetupPanel.cpp
    foundation/UserInputValidator.cpp
    function/FunctionSetupPanel.cpp
    function/MatlabFunctionSetupPanel.cpp
    hardware/BurnThrusterPanel.cpp
    hardware/PowerSystemConfigPanel.cpp
    hardware/ThrusterCoefficientDialog.cpp
    hardware/ThrusterConfigPanel.cpp
    mission/DecoratedTree.cpp
    mission/MissionTree.cpp
    mission/MissionTreeToolBar.cpp
    mission/TreeViewOptionDialog.cpp
    mission/UndockedMissionPanel.cpp
    output/CompareReportPanel.cpp
    output/EventFilePanel.cpp
    output/OutputTree.cpp
    output/ReportFilePanel.cpp
    propagator/PropagationConfigPanel.cpp
    propagator/PropagatorSelectDialog.cpp
    rendering/Camera.cpp
    rendering/GLStars.cpp
    rendering/GmatOpenGLSupport.cpp
    rendering/Light.cpp
    rendering/ModelManager.cpp
    rendering/ModelObject.cpp
    rendering/Rendering.cpp
    rendering/ThreeDSLoader.cpp
    solarsys/BarycenterPanel.cpp
    solarsys/CelesBodySelectDialog.cpp
    solarsys/CelestialBodyOrbitPanel.cpp
    solarsys/CelestialBodyOrientationPanel.cpp
    solarsys/CelestialBodyPanel.cpp
    solarsys/CelestialBodyPropertiesPanel.cpp
    solarsys/CelestialBodyVisualizationPanel.cpp
    solarsys/LibrationPointPanel.cpp
    solarsys/UniversePanel.cpp
    solver/DCSetupPanel.cpp
    solver/SQPSetupPanel.cpp
    solver/SolverCreatePanel.cpp
    solver/SolverGoalsPanel.cpp
    solver/SolverSetupPanel.cpp
    solver/SolverVariablesPanel.cpp
    spacecraft/AttitudePanel.cpp
    spacecraft/BallisticsMassPanel.cpp
    spacecraft/FormationSetupPanel.cpp
    spacecraft/OrbitDesignerDialog.cpp
    spacecraft/OrbitPanel.cpp
    spacecraft/OrbitSummaryDialog.cpp
    spacecraft/PowerSystemPanel.cpp
    spacecraft/SpacecraftPanel.cpp
    spacecraft/SpaceObjectSelectDialog.cpp
    spacecraft/TankPanel.cpp
    spacecraft/ThrusterPanel.cpp
    spacecraft/VisualModelCanvas.cpp
    spacecraft/VisualModelPanel.cpp
    subscriber/EphemerisFilePanel.cpp
    subscriber/GroundTrackCanvas.cpp
    subscriber/GroundTrackPlotPanel.cpp
    subscriber/MdiChild3DViewFrame.cpp
    subscriber/MdiChildGroundTrackFrame.cpp
    subscriber/MdiChildViewFrame.cpp
    subscriber/MdiChildTsFrame.cpp
    subscriber/MdiGlPlotData.cpp
    subscriber/MdiTsPlotData.cpp
    subscriber/OrbitViewCanvas.cpp
    subscriber/OrbitViewPanel.cpp
    subscriber/ReportFileSetupPanel.cpp
    subscriber/SubscriberSetupPanel.cpp
    subscriber/ViewCanvas.cpp
    subscriber/TsPlotCanvas.cpp
    subscriber/TsPlotXYCanvas.cpp
    subscriber/TsPlotCurve.cpp
    subscriber/TsPlotOptionsDialog.cpp
    subscriber/XyPlotSetupPanel.cpp
    view/EditorPanel.cpp
    view/EditorPreferences.cpp
    view/EditorPrintout.cpp
    view/FindReplaceDialog.cpp
    view/ScriptEditor.cpp
    view/ViewTextDialog.cpp
    view/ViewTextFrame.cpp
    ${SPICE_SRCS}
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE GUI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# TODO not sure I understand what are those mac specifics for... 
# it should no compromise building though...

if(APPLE)
#macrez:
#	echo "linking Carbon and wx resource files ......."
#	$(REZ) -d WXUSINGDLL -o $(TargetName)
#	cp -f $(TargetName) ../../application/bin/.

#macosx: 
#	echo "building Mac application bundle ......."
#	/Developer/Tools/SetFile -a C $(TOP_DIR)/application/bin/$(TargetName)
#	mkdir -p $(CONTENTS_DIR)
#	mkdir -p $(MACOS_DIR)
#	mkdir -p $(RES_DIR)
#	mkdir -p $(LIBS_DIR)
#	sed -e "s/IDENTIFIER/`echo GMAT | sed -e 's,\.\./,,g' | \
#	sed -e 's,/,.,g'`/" -e "s/EXECUTABLE/RunGMAT/" $(GMAT_INFO) >$(MAC_PKG)
#	echo -n "APPL????" > $(MAC_PKGINFO)
#	ln -f $(TOP_DIR)/application/bin/$(TargetName) $(MAC_APP)
#	ln -f $(TOP_DIR)/src/gui/RunGMAT $(MAC_SCRIPT_DIR)/RunGMAT
#	cp -f $(GMAT_ICONS) $(MAC_ICONS) 
#	./$(INSTALL_LIBS_INTO_BUNDLE) GMAT ../base/lib $(BASELIBNAME) $(PCRE_LIB_LOC)

#macstacksize:
#	echo "setting stack size needed for SPICE"
#	$(SPICE_STACKSIZE)
endif()

# ====================================================================
# add the icon information
SET(GMAT_ICON_DIR "resource")
SET(GMAT_ICON_O "GmatIcon.o")
if(WIN32)
  SET(GMAT_ICON_RC "GmatIcon.rc")
elseif(APPLE)
  SET(GMAT_ICON_RC "GMATMac.icns")
endif()
SET(GMAT_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_RC}")
SET(GMAT_ICON_O "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_O}")
SET(GUI_SRCS ${GUI_SRCS} ${GMAT_ICON_RC})

# ====================================================================
# add the install targets

# Create GmatGui as an executable
ADD_EXECUTABLE(${TargetName} ${GUI_SRCS} ${GUI_HEADERS})

# The GUI name should be just "GMAT"
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES OUTPUT_NAME "GMAT")

# The debug GUI should have the same postfix as top-levl CMakeLists.txt
# This is required because CMAKE_DEBUG_POSTFIX only applies to libraries
# and not executables as of CMake 3.0.2
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${GUI_DIRS}")

if(WIN32)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows")
endif()

# Create build products in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Specify where to install (make install or VS "INSTALL" project)
INSTALL( TARGETS ${TargetName}
  DESTINATION bin
  )
