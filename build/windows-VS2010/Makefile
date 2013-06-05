GMAT_HOME = C:/Users/builduser/Documents
GMAT_CURRENT_DIR = $(GMAT_HOME)/DailyBuildScript/VS
GMAT_DIR = $(GMAT_HOME)/GmatDevelopment
GMAT_SRC_BASE_DIR = $(GMAT_DIR)/src/base
GMAT_SRC_GUI_DIR = $(GMAT_DIR)/src/gui
GMAT_PLUGIN_DIR = $(GMAT_HOME)/GmatPlugins
GMAT_BUILD_DIR = $(GMAT_DIR)/build/windows-VS2010
GMAT_PLUGIN_BUILD_DIR = $(GMAT_HOME)/GmatPlugins/build/VS2010
GMAT_BIN_DIR = $(GMAT_DIR)/application/bin
GMAT_MATLAB_CINTF_DIR = $(GMAT_DIR)/application/matlab/libCInterface
GMAT_PLUGIN_BIN_DIR = $(GMAT_DIR)/application/plugins
GMAT_PROPRIETARY_BIN_DIR = $(GMAT_DIR)/application/plugins/proprietary
GMAT_CAPI_PLUGIN_DIR = $(GMAT_DIR)/plugins/CInterfacePlugin
GMAT_CAPI_PLUGIN_SRC_DIR = $(GMAT_CAPI_PLUGIN_DIR)/src
GMAT_CAPI_PLUGIN_DOCS_DIR = $(GMAT_CAPI_PLUGIN_DIR)/doc
GMAT_DOC_DIR = $(GMAT_DIR)/doc
GMAT_HELP_BUILD_DIR = $(GMAT_DOC_DIR)/help
#DESTINATION_DIR  = $(GMAT_HOME)/GmatBuilds
NETWORK_DIR = Y:
DESTINATION_DIR  = $(NETWORK_DIR)/Builds/windows/VS2010_build
BUILD_DATE = ` date "+%Y-%m-%d" `
DAILY_DESTINATION_DIR = $(DESTINATION_DIR)/$(BUILD_DATE)
DAILY_MATLAB_DEST_DIR = $(DAILY_DESTINATION_DIR)/matlab
DAILY_MATLAB_CINTF_DEST_DIR = $(DAILY_DESTINATION_DIR)/matlab/libCInterface
DAILY_EXE_DEST_DIR = $(DAILY_DESTINATION_DIR)/bin
DAILY_PLUGINS_DEST_DIR = $(DAILY_DESTINATION_DIR)/plugins
DAILY_PROPRIETARY_DEST_DIR = $(DAILY_DESTINATION_DIR)/plugins/proprietary
DAILY_DOCS_DEST_DIR = $(DAILY_DESTINATION_DIR)/docs
DAILY_HELP_DEST_DIR = $(DAILY_DOCS_DEST_DIR)/help
LATEST_BUILD_DIR = $(DESTINATION_DIR)/LatestCompleteVersion
LATEST_DOCS_BUILD_DIR = Y:/Builds/DevDocs

# compiles the latest build
all: 
	cd $(GMAT_BUILD_DIR); \
	msbuild.exe GmatVS2010.sln //p:Configuration=Release //t:Rebuild //fl ;
	cd $(GMAT_PLUGIN_BUILD_DIR); \
	msbuild.exe GmatPlugins.sln //p:Configuration=Release //t:Rebuild //fl ;

# cleans up the compiled source code and docs files
clean:
	cd $(GMAT_BUILD_DIR); \
	msbuild.exe GmatVS2010.sln //p:Configuration=Release //fl //target:Clean;
	cd $(GMAT_PLUGIN_BUILD_DIR); \
	msbuild.exe GmatPlugins.sln //p:Configuration=Release //fl //target:Clean;
	cd $(GMAT_BIN_DIR) && rm -rf *.dll
	cd $(GMAT_PLUGIN_BIN_DIR) && rm -rf *.dll
	cd $(GMAT_PROPRIETARY_BIN_DIR) && rm -rf *.dll
	cd $(GMAT_HELP_BUILD_DIR) && $(MAKE) clean

# checks out the repositories
checkout:
	cd $(GMAT_DIR); svn cleanup; svn update; 
	cd $(GMAT_PLUGIN_DIR); svn cleanup; svn update; 

# assembles the latest build, including user help
dist: 
	mkdir -p $(DAILY_DESTINATION_DIR); 
	mkdir -p $(DAILY_EXE_DEST_DIR); 
	mkdir -p $(DAILY_MATLAB_DEST_DIR); 
	mkdir -p $(DAILY_MATLAB_CINTF_DEST_DIR); 
	mkdir -p $(DAILY_PLUGINS_DEST_DIR); 
	mkdir -p $(DAILY_PROPRIETARY_DEST_DIR); 
	mkdir -p $(DAILY_DOCS_DEST_DIR); 
	mkdir -p $(DAILY_HELP_DEST_DIR); 
	cd $(GMAT_BIN_DIR); \
	cp GMAT.exe $(DAILY_EXE_DEST_DIR); \
	cp libGmatBase.dll $(DAILY_EXE_DEST_DIR); \
	cp libCInterface.dll $(DAILY_EXE_DEST_DIR); 
	cd $(GMAT_MATLAB_CINTF_DIR); \
	cp interfacewrapper.m $(DAILY_MATLAB_CINTF_DEST_DIR); 
	cd $(GMAT_PLUGIN_BIN_DIR); \
	cp libMatlabInterface.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libFminconOptimizer.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libGmatEstimation.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libDataInterface.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libEphemPropagator.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libEventLocator.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libExtraPropagators.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libFormation.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libGmatFunction.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libSaveCommand.dll $(DAILY_PLUGINS_DEST_DIR); \
	cp libStation.dll $(DAILY_PLUGINS_DEST_DIR);
	cd $(GMAT_PROPRIETARY_BIN_DIR); \
	cp libMsise86.dll $(DAILY_PROPRIETARY_DEST_DIR); \
	cp libNewParameters.dll $(DAILY_PROPRIETARY_DEST_DIR); \
	cp libNRLMsise00.dll $(DAILY_PROPRIETARY_DEST_DIR); \
	cp libMarsGRAM.dll $(DAILY_PROPRIETARY_DEST_DIR); \
	cp libVF13Optimizer.dll $(DAILY_PROPRIETARY_DEST_DIR); 
	cd $(GMAT_HELP_BUILD_DIR); \
	cp help-a4.pdf $(DAILY_HELP_DEST_DIR); \
	cp help-letter.pdf $(DAILY_HELP_DEST_DIR); \
	cp help.html $(DAILY_HELP_DEST_DIR); \
	cp help.chm $(DAILY_HELP_DEST_DIR); \
	cp -r $(GMAT_HELP_BUILD_DIR)/html $(DAILY_HELP_DEST_DIR);  \
	cp -r $(GMAT_HELP_BUILD_DIR)/files $(DAILY_HELP_DEST_DIR); 
	rm -rf $(LATEST_BUILD_DIR);
	$(GMAT_BUILD_DIR)/assemble.sh -d $(LATEST_BUILD_DIR) -j $(GMAT_PLUGIN_DIR) -n $(NETWORK_DIR) -b $(DAILY_DESTINATION_DIR)

dist-devdocs:
	rm -rf $(LATEST_DOCS_BUILD_DIR)
	mkdir -p $(LATEST_DOCS_BUILD_DIR)
	cp -r $(GMAT_DOC_DIR)/html/base $(LATEST_DOCS_BUILD_DIR)
	cp -r $(GMAT_DOC_DIR)/html/gui $(LATEST_DOCS_BUILD_DIR)
	cp -r $(GMAT_CAPI_PLUGIN_DOCS_DIR)/capi $(LATEST_DOCS_BUILD_DIR)

check:

# makes the user docs
userdocs:
	cd  $(GMAT_HELP_BUILD_DIR) && $(MAKE) help

# makes the dev docs
devdocs:
	cd $(GMAT_SRC_BASE_DIR); doxygen;
	cd $(GMAT_SRC_GUI_DIR); doxygen;
	cd $(GMAT_CAPI_PLUGIN_SRC_DIR); doxygen;

