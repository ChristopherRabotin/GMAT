GMAT_HOME = C:/GmatVS
GMAT_DIR = $(GMAT_HOME)/GmatDevelopment
GMAT_PLUGIN_DIR = $(GMAT_HOME)/GmatPlugins
GMAT_BUILD_DIR = $(GMAT_DIR)/build/windows-VS2010
GMAT_PLUGIN_BUILD_DIR = $(GMAT_HOME)/GmatPlugins/build/VS2010
GMAT_BIN_DIR = $(GMAT_DIR)/application/bin
GMAT_PLUGIN_BIN_DIR = $(GMAT_DIR)/application/plugins
DESTINATION_DIR  = $(GMAT_HOME)/GmatBuilds
#DESTINATION_DIR  = N:/Builds/windows/VS2010_build
BUILD_DATE = ` date "+%Y-%m-%d" `
DAILY_DESTINATION_DIR = $(DESTINATION_DIR)/$(BUILD_DATE)

all:
	cd $(GMAT_BIN_DIR); rm -rf *.dll; \
	cd $(GMAT_DIR); svn update; \
	cd $(GMAT_PLUGIN_DIR); svn update; \
	cd $(GMAT_BUILD_DIR); \
	msbuild.exe GmatVS2010.sln //p:Configuration=ReleaseWithSPICE //t:Rebuild //fl ;\
	mkdir -p $(DAILY_DESTINATION_DIR); \
	cd $(GMAT_BIN_DIR); \
	cp GMAT.exe $(DAILY_DESTINATION_DIR); \
	cp libGmatBase.dll $(DAILY_DESTINATION_DIR); \
	cp libCInterface.dll $(DAILY_DESTINATION_DIR); \
	cd $(GMAT_PLUGIN_BUILD_DIR); \
	msbuild.exe GmatPlugins.sln //p:Configuration=Release //t:Rebuild //fl ;\
	cd $(GMAT_PLUGIN_BIN_DIR); \
	cp libMatlabInterface.dll $(DAILY_DESTINATION_DIR); \
	cp libFminconOptimizer.dll $(DAILY_DESTINATION_DIR); \
	cp libDataFile.dll $(DAILY_DESTINATION_DIR); \
	cp libGmatEstimation.dll $(DAILY_DESTINATION_DIR); \
	cp libCcsdsEphemerisFile.dll $(DAILY_DESTINATION_DIR); \
	cd $(GMAT_PLUGIN_BIN_DIR)/proprietary; \
	cp libVF13Optimizer.dll $(DAILY_DESTINATION_DIR); \
