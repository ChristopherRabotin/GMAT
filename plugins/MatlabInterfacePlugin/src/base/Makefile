# $Id: Makefile 9350 2011-03-31 18:30:12Z wendys-dev $
# makefile for GMAT's MatlabInterface Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code
# This make file currently assumes wxMac version 2.8.x

include ../MatlabInterfaceEnv.mk


TARGET = ../../lib/libMatlabInterface$(SHARED_EXTENSION)


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Iinclude \
          -Iplugin \
          -Ifactory \
          -Ifunction \
          -Iinterface \
          -Isubscriber \
          -Icommand \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/util \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/function \
          -I$(GMAT_CODE_LOCATION)/base/parameter \
          -I$(GMAT_CODE_LOCATION)/base/solver \
          -I$(GMAT_CODE_LOCATION)/base/subscriber \
          -I$(GMAT_CODE_LOCATION)/base/interface \
          -I$(GMAT_CODE_LOCATION)/base/command \
          -I$(GMAT_CODE_LOCATION)/base/executive \
          -I$(GMAT_CODE_LOCATION)/base/interpreter \
          -I$(GMAT_CODE_LOCATION)/base/forcemodel \
          -I$(GMAT_CODE_LOCATION)/gui/include \
          -I$(GMAT_CODE_LOCATION)/gui/app 


clean : archclean

archclean :
	rm -rf */*.o
	rm -rf $(MEX_OBJECTS)
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

OBJECTS = \
    plugin/GmatPluginFunctions.o \
    factory/MatlabInterfaceFactory.o \
    factory/MatlabWorkspaceFactory.o \
    factory/CallMatlabFunctionFactory.o \
    factory/MatlabFunctionFactory.o \
    interface/MatlabInterface.o \
    subscriber/MatlabWorkspace.o \
    command/CallMatlabFunction.o \
    function/MatlabFunction.o \
	 $(MEX_OBJECTS)

ifeq ($(LINUX_MAC), 1)
ifeq ($(MAC_SPECIFIC), 1)
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	install_name_tool -change lib/lib$(BASE_LIBRARY).dylib $(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_richtext-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_richtext-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_aui-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_aui-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_xrc-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_xrc-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_qa-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_qa-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_html-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_html-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_adv-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_adv-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_mac_core-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_mac_core-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_base_carbon_xml-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_base_carbon_xml-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_base_carbon_net-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_base_carbon_net-2.8.0.dylib $(TARGET)
	install_name_tool -change /usr/local/lib/libwx_base_carbon-2.8.0.dylib $(BUNDLE_LIB_LOCATION)libwx_base_carbon-2.8.0.dylib $(TARGET)
	cp $(TARGET) $(GMAT_PLUGIN_LOCATION)
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS) $(MATLAB_LIB_DIR) $(MATLAB_LIBRARIES)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
endif	
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
endif


$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
    
