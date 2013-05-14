# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GUI
# This file must be installed in the src/gui directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
# 

MESSAGE("==============================")
MESSAGE("GMAT GUI building setup " ${VERSION})


# ====================================================================
SET(GUITARGET "GmatGui")


# ====================================================================
# DEVIL package

if (NOT WIN32)
    MESSAGE("-- Building with DeVIL")

    if (NOT DevIL_DIR)
        find_package(DevIL)

        if(NOT IL_FOUND)
            MESSAGE("** Disabling USE_DEVIL")
            SET(USE_DEVIL FALSE)
        endif()
    else()
        # check directory
        if(EXISTS "${DevIL_DIR}" AND IS_DIRECTORY "${DevIL_DIR}")
            SET(IL_INCLUDE_DIR ${DevIL_DIR})
        else()
            MESSAGE("** DEVIL NOT FOUND in directory " ${DevIL_DIR})
            MESSAGE("** Disabling USE_DEVIL")
            SET(USE_DEVIL FALSE)
        endif()
    endif ()
    

    if (USE_DEVIL)
        MESSAGE("-- DevIL FOUND")
        MESSAGE("-- DevIL_DIR=" ${IL_INCLUDE_DIR})
        LINK_DIRECTORIES(${IL_INCLUDE_DIR})
        LINK_LIBRARIES(${IL_LIBRARIES})
        LINK_LIBRARIES(${ILU_LIBRARIES})
        LINK_LIBRARIES(${ILUT_LIBRARIES})
        INCLUDE_DIRECTORIES(${IL_INCLUDE_DIR})
    else()
        ADD_DEFINITIONS("-DSKIP_DEVIL")
    endif()
endif()


# ====================================================================
# SPICE
if(64_BIT)
	SET(CSPICE_DIR "${CSPICE_DIR}/cspice64")
else()
	SET(CSPICE_DIR "${CSPICE_DIR}/cspice32")
endif()

INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")
SET(SPICE_SRCS "spacecraft/SpicePanel.cpp")

# ====================================================================
# other libraries
LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../application/bin")
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()

# Add Preprosser Definitions for wx widgets
ADD_DEFINITIONS("-D__USE_WX280__")
ADD_DEFINITIONS("-D__USE_WX280_GL__")
ADD_DEFINITIONS("-DwxUSE_GLCANVAS")

# need some opengl libs
if(WIN32)
    LINK_LIBRARIES("glu32")
    LINK_LIBRARIES("opengl32")
elseif(APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Carbon -framework OpenGL -framework AGL")
    
    INCLUDE_DIRECTORIES ( /System/Library/Frameworks/OpenGL.framework )
    
    FIND_LIBRARY(GLU_LIBRARY Glu )
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
    
    MESSAGE(${OpenGL_LIBRARY})
    MESSAGE(${GLU_LIBRARY})
    
    LINK_LIBRARIES(${GLU_LIBRARY})
	LINK_LIBRARIES(${OpenGL_LIBRARY})
    
    #SET(EXTRA_LIBS ${GLU_LIBRARY} ${OpenGL_LIBRARY})
    #target_link_libraries(${GUITARGET} ${EXTRA_LIBS})
    #LINK_LIBRARIES("GLU")
else()
	LINK_LIBRARIES("GL")
	LINK_LIBRARIES("GLU")
endif()

ADD_DEFINITIONS("-D_DYNAMICLINK")

if(WIN32)
	ADD_DEFINITIONS("-DWXUSINGDLL")
elseif(APPLE)
    SET (WX_COMPILE_FLAGS "-I/${WXWIDGET_DIR}/lib/wx/include/mac-ansi-release-2.8 -D__WXMAC__")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_COMPILE_FLAGS}")
else()
	SET (WX_COMPILE_FLAGS "-I/${WXWIDGET_DIR}/lib/wx/include/gtk2-ansi-release-2.8 -D__WXGTK__")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_COMPILE_FLAGS}")
endif()

ADD_DEFINITIONS("-D_MBCS")
ADD_DEFINITIONS("-D__USE_STC_EDITOR__")
if(WIN32)
	ADD_DEFINITIONS("-D__ENABLE_EMAIL__")
endif()
ADD_DEFINITIONS("-D__USE_SPICE__")


# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES("../base/include")
INCLUDE_DIRECTORIES("../base/asset")
INCLUDE_DIRECTORIES("../base/attitude")
INCLUDE_DIRECTORIES("../base/burn")
INCLUDE_DIRECTORIES("../base/command")
INCLUDE_DIRECTORIES("../base/configs")
INCLUDE_DIRECTORIES("../base/coordsystem")
INCLUDE_DIRECTORIES("../base/executive")
INCLUDE_DIRECTORIES("../base/factory")
INCLUDE_DIRECTORIES("../base/forcemodel")
INCLUDE_DIRECTORIES("../base/foundation")
INCLUDE_DIRECTORIES("../base/function")
INCLUDE_DIRECTORIES("../base/hardware")
INCLUDE_DIRECTORIES("../base/interface")
INCLUDE_DIRECTORIES("../base/interpolator")
INCLUDE_DIRECTORIES("../base/interpreter")
INCLUDE_DIRECTORIES("../base/math")
INCLUDE_DIRECTORIES("../base/parameter")
INCLUDE_DIRECTORIES("../base/plugin")
INCLUDE_DIRECTORIES("../base/propagator")
INCLUDE_DIRECTORIES("../base/solarsys")
INCLUDE_DIRECTORIES("../base/solver")
INCLUDE_DIRECTORIES("../base/spacecraft")
INCLUDE_DIRECTORIES("../base/stopcond")
INCLUDE_DIRECTORIES("../base/subscriber")
INCLUDE_DIRECTORIES("../base/util")
INCLUDE_DIRECTORIES("../base/forcemodel")
INCLUDE_DIRECTORIES("../base/forcemodel/harmonic")
INCLUDE_DIRECTORIES("../base/event")
INCLUDE_DIRECTORIES("include")
INCLUDE_DIRECTORIES("include/bitmaps")
INCLUDE_DIRECTORIES("include/il")
INCLUDE_DIRECTORIES("app")
INCLUDE_DIRECTORIES("asset")
INCLUDE_DIRECTORIES("burn")
INCLUDE_DIRECTORIES("command")
INCLUDE_DIRECTORIES("controllogic")
INCLUDE_DIRECTORIES("coordsystem")
INCLUDE_DIRECTORIES("forcemodel")
INCLUDE_DIRECTORIES("foundation")
INCLUDE_DIRECTORIES("function")
INCLUDE_DIRECTORIES("hardware")
INCLUDE_DIRECTORIES("mission")
INCLUDE_DIRECTORIES("output")
INCLUDE_DIRECTORIES("propagator")
INCLUDE_DIRECTORIES("rendering")
INCLUDE_DIRECTORIES("solarsys")
INCLUDE_DIRECTORIES("solver")
INCLUDE_DIRECTORIES("spacecraft")
INCLUDE_DIRECTORIES("subscriber")
INCLUDE_DIRECTORIES("view")
INCLUDE_DIRECTORIES("./event")
INCLUDE_DIRECTORIES(${WXWIDGET_DIR}/include/)
INCLUDE_DIRECTORIES(${WXWIDGET_DIR}/include/msvc/)
INCLUDE_DIRECTORIES(${WXWIDGET_DIR}/contrib/include)

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
    spacecraft/SpacecraftPanel.cpp
    spacecraft/SpaceObjectSelectDialog.cpp
    spacecraft/TankPanel.cpp
    spacecraft/ThrusterPanel.cpp
    spacecraft/VisualModelCanvas.cpp
    spacecraft/VisualModelPanel.cpp
    subscriber/EphemerisFilePanel.cpp
	subscriber/GroundTrackCanvas.cpp
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
	view/Editor.cpp
	view/EditorPanel.cpp
	view/EditorPreferences.cpp
	view/EditorPrintout.cpp
    view/FindReplaceDialog.cpp
    view/ViewTextDialog.cpp
    view/ViewTextFrame.cpp
    ${STC_EDITOR_SRCS}
    ${SPICE_SRCS}
)


# ====================================================================
# TODO not sure I understand what are those mac specifics for... 
# it should no compromise building though...

if(APPLE)
#macrez:
#	echo "linking Carbon and wx resource files ......."
#	$(REZ) -d WXUSINGDLL -o $(GUITARGET)
#	cp -f $(GUITARGET) ../../application/bin/.

#macosx: 
#	echo "building Mac application bundle ......."
#	/Developer/Tools/SetFile -a C $(TOP_DIR)/application/bin/$(GUITARGET)
#	mkdir -p $(CONTENTS_DIR)
#	mkdir -p $(MACOS_DIR)
#	mkdir -p $(RES_DIR)
#	mkdir -p $(LIBS_DIR)
#	sed -e "s/IDENTIFIER/`echo GMAT | sed -e 's,\.\./,,g' | \
#	sed -e 's,/,.,g'`/" -e "s/EXECUTABLE/RunGMAT/" $(GMAT_INFO) >$(MAC_PKG)
#	echo -n "APPL????" > $(MAC_PKGINFO)
#	ln -f $(TOP_DIR)/application/bin/$(GUITARGET) $(MAC_APP)
#	ln -f $(TOP_DIR)/src/gui/RunGMAT $(MAC_SCRIPT_DIR)/RunGMAT
#	cp -f $(GMAT_ICONS) $(MAC_ICONS) 
#	./$(INSTALL_LIBS_INTO_BUNDLE) GMAT ../base/lib $(BASELIBNAME) $(PCRE_LIB_LOC)

#macstacksize:
#	echo "setting stack size needed for SPICE"
#	$(SPICE_STACKSIZE)
endif()

# ====================================================================
# add the icon, windows only.
if(WIN32)
    SET(GMAT_ICON_RC  "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_RC}")
    SET(GMAT_ICON_O  "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_O}")
    SET(GUI_SRCS "${GUI_SRCS};${GMAT_ICON_RC}")
elseif(APPLE)
# we can add an icon for mac/gmat

endif()

# ====================================================================
# compilation

# add the install targets
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../application/bin")

if(WIN32)
	if(64_BIT)
		SET(WXCONTRIB_LIB_DIR "${WXWIDGET_DIR}/lib/vc_amd64_dll")
		LINK_DIRECTORIES("${WXWIDGET_DIR}/lib/vc_amd64_dll")
	else()
		SET(WXCONTRIB_LIB_DIR "${WXWIDGET_DIR}/lib/vc_dll")
		LINK_DIRECTORIES("${WXWIDGET_DIR}/lib/vc_dll")
	endif()

	LINK_LIBRARIES("${WXCONTRIB_LIB_DIR}/wxmsw28_netutils.lib")
	LINK_LIBRARIES("${WXCONTRIB_LIB_DIR}/wxmsw28_stc.lib")
endif()

ADD_EXECUTABLE(${GUITARGET} ${GUI_SRCS})

if(UNIX)
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_core-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_richtext-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_aui-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_xrc-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_qa-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_html-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_adv-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_stc-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base_xml-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base_net-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base-2.8.so")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_gl-2.8.so")
	#target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_netutils-2.8.so")	//Does not build on linux/mac
elseif(APPLE)
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_core-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_richtext-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_aui-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_xrc-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_qa-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_html-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_adv-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_stc-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base_carbon_xml-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base_carbon_net-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_base_carbon-2.8.dylib")
	target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_mac_gl-2.8.dylib")
	#target_link_libraries(${GUITARGET} "/${WXWIDGET_DIR}/lib/libwx_gtk2_netutils-2.8.dylib")	//Does not build on linux/mac
endif()

if(WIN32)
	SET_TARGET_PROPERTIES(${GUITARGET} PROPERTIES TARGET_VERSION_MAJOR "${VERSION}" LINK_FLAGS "/SUBSYSTEM:windows")
else()
	SET_TARGET_PROPERTIES(${GUITARGET} PROPERTIES TARGET_VERSION_MAJOR "${VERSION}")
endif()
